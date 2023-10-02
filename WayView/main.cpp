// main.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include "pch.h"
#include <iostream>
#include <cstring>
#include <vector>

#include <locale.h>
#include <wchar.h>
#include <iostream>

//https://stackoverflow.com/questions/27759754/qt-5-4-static-build-produces-unresolved-external-symbol-link-error-in-visual-s
#include "QtCore/qplugin.h"
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#include "my_window_QT.h"

//#include <Windows.h>

using namespace cv;

#define MY_UNREFERENCED_PARAMETER(p) p


//For some reason keyPressEvent doesn't detect arrow keys (and cvWaitKey doesn't detect arrow keys).
//As an alternative, we use Qt events, and mycvWaitKey returns a unique value when an arrow key is pressed.
enum class ARROW_KEY //C++11 scoped enum
{
	LEFT	= 1000001,
	RIGHT	= 1000002,
	UP		= 1000003,
	DOWN	= 1000004
};


enum class F_KEY //C++11 scoped enum
{
	F5 = 116
};



//Open a message box showing an error message, and exit
//https://stackoverflow.com/a/10017591/4926757
static void showErrorMessageAndExit(const std::wstring msg, bool do_create_application = false)
{
    if (do_create_application)
    {
        //When this function is executed before creating a "named window", we must create a QApplication before showing a message box.
        //https://stackoverflow.com/a/21595802/4926757
        int argc = 1;
        QApplication app(argc, nullptr);
        QMessageBox messageBox;
        //messageBox.setIcon(QMessageBox::Critical);
        app.setWindowIcon(QIcon(":/way_view-icon"));
        messageBox.critical(0, "Error", QString::fromStdWString(msg));
        exit(1);
    }
    else
    {
        QMessageBox messageBox;
        //messageBox.setIcon(QMessageBox::Critical);
        messageBox.setWindowIcon(QIcon(":/way_view-icon"));
        messageBox.critical(0, "Error", QString::fromStdWString(msg));
        exit(1);
    }
}



//Read image from file with wchar_t name
//In case image has 4 color channels, assume BGRA color space, and convert from BGRA to BGR (the application doesn't support BGRA format).
static Mat myimread(const wchar_t filename[], int flags = 1, bool do_create_application_when_showing_error = false)
{
	FILE *f;
	uint64_t file_size;
	_wfopen_s(&f, filename, L"rb");

	if (f == nullptr)
	{
        showErrorMessageAndExit(std::wstring(L"Error: can't open file: ") + filename, do_create_application_when_showing_error);
	}

	_fseeki64(f, 0, SEEK_END);
	file_size = (uint64_t)_ftelli64(f);
	_fseeki64(f, 0, SEEK_SET);

	if (file_size > MAXINT32)
	{
        showErrorMessageAndExit(std::wstring(L"Error: file size is too large: ") + filename + L", (" + std::to_wstring(file_size) + L"bytes)", do_create_application_when_showing_error);
	}

	unsigned char *raw = new unsigned char[(size_t)file_size];

	if (raw == nullptr)
	{
        showErrorMessageAndExit(L"Error: out of memory", do_create_application_when_showing_error);
	}

	//Read file to memory buffer.
	fread_s(raw, (size_t)file_size, 1, (size_t)file_size, f);

	fclose(f);


	//https://stackoverflow.com/questions/14727267/opencv-read-jpeg-image-from-buffer
	// Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
	Mat rawData(1, (int)file_size, CV_8UC1, (void*)raw);

	Mat decodedImage = imdecode(rawData, flags);

	if (decodedImage.data == nullptr)
	{
		// Error reading raw image data
        showErrorMessageAndExit(std::wstring(L"Error decoding file ") + filename, do_create_application_when_showing_error);
	}

	delete[] raw;

    if (decodedImage.channels() == 4)
    {
        //Convert from BGRA to BGR if input image has 4 color channels.
        cvtColor(decodedImage, decodedImage, CV_BGRA2BGR);
    }

	return decodedImage;
}




class CListOfFiles
{
private:
	//https://stackoverflow.com/questions/36494584/find-string-in-vector-of-strings-case-insensitive-c
	int VecFindIgnoreCase(const std::vector<std::wstring>& vec, const wchar_t* sFind)
	{
		for (std::vector<std::wstring>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			if (_wcsicmp((*iter).c_str(), sFind) == 0)
			{
				return (int)std::distance(vec.begin(), iter);
			}
		}

		return -1;
	}

	int VecFindIgnoreCase(const std::vector<std::wstring>& vec, const std::wstring& sFind)
	{
		return VecFindIgnoreCase(vec, sFind.c_str());
	}


public:
	std::vector<std::wstring> Q;

	CListOfFiles() {}

	~CListOfFiles() {}

	//Build vector of files matching <fnames> (example: if fnames is C:\Tmp\*.tif, fill Q with all tif file names in C:\Tmp folder)
	int dir(std::wstring fnames)
	{
		Q.clear();

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFileExW(fnames.c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);;

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		Q.push_back(std::wstring(FindFileData.cFileName));

		while (FindNextFileW(hFind, &FindFileData))
		{
			Q.push_back(std::wstring(FindFileData.cFileName));
		}

		FindClose(hFind);

		return (int)Q.size();
	}


	//Return index of fname in vector Q, return (-1) if not exist (case insensitive search).
	int findFileIdx(std::wstring fname)
	{
		int idx;

		idx = VecFindIgnoreCase(Q, fname);

		return idx;
	}


	//Return name of previous file in vector Q.
	std::wstring getPrevFileName(std::wstring fname)
	{
		int idx;
		int len = (int)Q.size();

		idx = VecFindIgnoreCase(Q, fname);

		if (idx == -1)
		{
			//Return empty string if fname is not is Q.
			return std::wstring();
		}

		if (idx == 0)
		{
			//If fname is the first file in the list, return the last file
			return Q[len - 1];
		}
		else
		{
			//Return previous file name from the list
			return Q[idx - 1];
		}
	}


	//Return name of next file in vector Q.
	std::wstring getNextFileName(std::wstring fname)
	{
		int idx;
		int len = (int)Q.size();

		idx = VecFindIgnoreCase(Q, fname);

		if (idx == -1)
		{
			//Return empty string if fname is not is Q.
			return std::wstring();
		}

		if (idx >= (len-1))
		{
			//If fname is the last file in the list, return the first.
			return Q[0];
		}
		else
		{
			//Return next file name from the list
			return Q[idx + 1];
		}
	}
};



//Collect 3 histograms (or red, green, blue channels) of uint16 image I (BGR channel ordering)
//Histograms must be initialize to zeros when executing the function.
//Size of histograms must be 2^16 = 65536 elements.
static void calcHistBgr16bit(const Mat I,
                             std::vector<int> &rHvec,
                             std::vector<int> &gHvec,
                             std::vector<int> &bHvec)
{
    const unsigned short *A = (unsigned short*)I.data;
    size_t stride = I.step[0];  //Rows stride in bytes.

    //Use pointers to "data" instead of vectors for improving execution performance (disable the index range checking of std::vector type).
    int *rH = (int*)rHvec.data();
    int *gH = (int*)gHvec.data();
    int *bH = (int*)bHvec.data();

    for (int y = 0; y < I.rows; y++)
    {
        const unsigned short *A0 = (unsigned short*)((unsigned char*)A + y*stride); //Points the beginning of the row

        for (int x = 0; x < I.cols; x++)        
        {
            int b0 = (int)A0[x*3];    //Blue element
            int g0 = (int)A0[x*3+1];  //Green element
            int r0 = (int)A0[x*3+2];  //Red element

            rH[r0]++;
            gH[g0]++;
            bH[b0]++;
        }
    }
}


//Collect histogram of uint16 image single channel (Grayscale) image I.
//Histogram must be initialize to zeros when executing the function.
//Size of histogram must be 2^16 = 65536 elements.
static void calcHistGrayscale16bit(const Mat I,
                                   std::vector<int> &Hvec)
{
    const unsigned short *A = (unsigned short*)I.data;
    size_t stride = I.step[0];  //Rows stride in bytes.

    //Use pointers to "data" instead of vectors for improving execution performance (disable the index range checking of std::vector type).
    int *H = (int*)Hvec.data();

    for (int y = 0; y < I.rows; y++)
    {
        const unsigned short *A0 = (unsigned short*)((unsigned char*)A + y*stride); //Points the beginning of the row

        for (int x = 0; x < I.cols; x++)        
        {
            int a0 = (int)A0[x];    //Grayscale element
            H[a0]++;
        }
    }
}


//Compute upper percentile and lower percentile of an image by the given histogram of the image.
//Computing percentile using the histogram is much more efficient compared to sorting the elements of the image for computing the percentile.
static void calcUpperAndLowerPercentileByHist(const std::vector<int> &Hvec, //Histogram of the original image (full histogram - e.g 2^16 elements for uint16 image).
                                              const double lower_percentile, //i.e 0.25 for 0.25% lower percentile
                                              const double upper_percentile, //i.e 0.25 for 0.25% upper percentile
                                              const uint64_t hist_sum,  //Equals rows*cols of original image 
                                              int *lower_val,
                                              int *upper_val)
                                              
{
    const uint64_t lo_thresh = std::round(lower_percentile*0.01*(double)hist_sum); //Lower percentile cumulative sum threshold.
    const uint64_t up_thresh = std::round((100.0-upper_percentile)*0.01*(double)hist_sum); //Upper percentile cumulative sum threshold.
    int hist_len = (int)Hvec.size();

    //Use pointers to "data" instead of vectors for improving execution performance (disable the index range checking of std::vector type).
    int *H = (int*)Hvec.data();

    int lower_value = -1;
    int upper_value = -1;
    uint64_t cum_sum = 0;

    for (int i = 0; i < hist_len; i++)
    {
        cum_sum += H[i];
        if ((cum_sum >= lo_thresh) && (lower_value == -1))
        {
            lower_value = i;    //Value that lower_percentile percent of the pixels are below it.
        }

        if ((cum_sum >= up_thresh) && (upper_value == -1))
        {
            upper_value = i;    //Value that (100-upper_percentile) percent of the pixels are above it.
        }
    }

    if (lower_value == -1)
    {
        lower_value = 0;    //Protection.
    }

    if (upper_value == -1)
    {
        upper_value = 0;    //Protection.
    }

    *lower_val = lower_value;
    *upper_val = upper_value;
}
                                              



//Process image for display.
// - If I has 8 bits per channel (8bpc Grayscale of 8bpc BGR, keep the original image as is).
// - If I is 16 bits Grayscale, apply linear stretching to range [0, 255], and convert to uint8.
// - If I is 16 bits colored (BGR), apply linear stretching to each color channel to [0, 255], and convert to uint8
//   Stretch Red color channel, Green color channel and Blue color channel independently.
static Mat processImageForDisplay(const Mat I)
{
    Mat J;

	if (I.depth() == CV_8U)
	{
		I.copyTo(J);
	}
	else
	{
        const double lower_percentile = 0.25;   //i.e 0.25 applies 0.25% lower percentile
        const double upper_percentile = 0.25;   //i.e 0.25 applies 0.25% upper percentile
      
        bool is_colored = (I.channels() == 3);

        uint64_t hist_sum = (uint64_t)I.rows * (uint64_t)I.cols;    //Histogram sum equals the total number of pixels in the image.

        if (is_colored)
        {
            int lo_r, lo_g, lo_b, up_r, up_g, up_b;
            std::vector<int> rH = std::vector<int>(65536, 0);   //Allocate a vector and initialize to zeros.
            std::vector<int> gH = std::vector<int>(65536, 0);
            std::vector<int> bH = std::vector<int>(65536, 0);
            calcHistBgr16bit(I, rH, gH, bH);    //Collect 3 histograms

            //Compute upper and lower percentiles of each color channel
            calcUpperAndLowerPercentileByHist(rH, lower_percentile, upper_percentile, hist_sum, &lo_r, &up_r);
            calcUpperAndLowerPercentileByHist(gH, lower_percentile, upper_percentile, hist_sum, &lo_g, &up_g);
            calcUpperAndLowerPercentileByHist(bH, lower_percentile, upper_percentile, hist_sum, &lo_b, &up_b);
            
            double r_scale = (up_r == lo_r) ? 1.0 : (255.0 / (double)(up_r - lo_r)); //After subtracting lo_r, the upper percentile goes to 255.
            double g_scale = (up_g == lo_g) ? 1.0 : (255.0 / (double)(up_g - lo_g));
            double b_scale = (up_b == lo_b) ? 1.0 : (255.0 / (double)(up_b - lo_b));
            double r_offs = (double)(-lo_r)*r_scale; //We want to subtract lower percentile from each color channel (going to be 0 in J).
            double g_offs = (double)(-lo_g)*g_scale;
            double b_offs = (double)(-lo_b)*b_scale;
            
            Mat dstB, dstG, dstR;
            Mat src_bgr_channels[3];            
            cv::split(I, src_bgr_channels); //Split BGR channels to B, G and R.
            const Mat srcB = src_bgr_channels[0];
            const Mat srcG = src_bgr_channels[1];
            const Mat srcR = src_bgr_channels[2];

            srcR.convertTo(dstR, CV_8U, r_scale, r_offs);   //dstR = saturate_cast<CV_8U>(r_scale*srcR + r_offs)
            srcG.convertTo(dstG, CV_8U, g_scale, g_offs);
            srcB.convertTo(dstB, CV_8U, b_scale, b_offs);

            std::vector<Mat> dst_bgr_channels = {dstB, dstG, dstR};
            cv::merge(dst_bgr_channels, J); //Merge destination B,G,R channels to J (3 color channels BGR format)
        }
        else
        {
            int lo, up;
            std::vector<int> H = std::vector<int>(65536, 0);
            calcHistGrayscale16bit(I, H);       //Collect 1 histogram

            //Compute upper and lower percentiles
            calcUpperAndLowerPercentileByHist(H, lower_percentile, upper_percentile, hist_sum, &lo, &up);

            //We want to subtract lower percentile from I (going to be 0 in J).            
            double scale = (up == lo) ? 1.0 : (255.0 / (double)(up - lo)); //After subtracting lo, the upper percentile goes to 255.
            double offs = (double)(-lo)*scale;

            I.convertTo(J, CV_8U, scale, offs);  //J = saturate_cast<CV_8U>(scale*I + offs)
        }

		//Mat T = I / 100;T.convertTo(J, CV_8U); //Test
	}

    return J;
}



//int wmain(int argc, wchar_t *argv[])
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    MY_UNREFERENCED_PARAMETER(hInstance);
    MY_UNREFERENCED_PARAMETER(hPrevInstance);
    MY_UNREFERENCED_PARAMETER(lpCmdLine);
    MY_UNREFERENCED_PARAMETER(nCmdShow);

	errno_t sts = (errno_t)0;

    //https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw?redirectedfrom=MSDN
    //Convert command line arguments to argc and argv
    ////////////////////////////////////////////////////////////////////////////
    wchar_t **argv;
    int argc;

    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (nullptr == argv)
    {
        showErrorMessageAndExit(L"CommandLineToArgvW failed", true);
    }    
    ////////////////////////////////////////////////////////////////////////////


	if (argc != 2)
	{
        if (argc > 2)
        {
            showErrorMessageAndExit(L"Too many input arguments, usage: WayView image_file_name", true);
        }
        else
        {
            showErrorMessageAndExit(L"Missing input argument, usage: WayView image_file_name\n Example: WayView myimage.tif", true);
        }
	}

	const wchar_t *path = argv[1];
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[4096];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	wchar_t wpath_buffer[4096];
	wchar_t tiffs_path[4096 + _MAX_EXT];    //The name "tiffs_path" is used for path\*.tif, but may also be other file type like path\*.png

	const wchar_t winname[] = L"I";

	int balance_select = 0;


	memset(wpath_buffer, 0, sizeof(wpath_buffer));

    //Split image file path to drive, directory, file name and file extension
	sts = _wsplitpath_s(path,			    //const wchar_t * path,
					    drive,				//wchar_t * drive,
					    _countof(drive),	//size_t driveNumberOfElements,
					    dir,				//wchar_t * dir,
					    _countof(dir),		//size_t dirNumberOfElements,
					    fname,				//wchar_t * fname,
					    _countof(fname),	//size_t nameNumberOfElements,
					    ext,				//wchar_t * ext,
					    _countof(ext));		//size_t extNumberOfElements);

	if (sts != 0)
	{
        showErrorMessageAndExit(std::wstring(L"_splitpath_s return error code ") + std::to_wstring((int)sts), true);
	}

    //Make path from drive, dir, fname and ext (splitting an making the path removes any odd parts like quotes).
	_wmakepath_s(wpath_buffer,				//wchar_t *path,
				 _countof(wpath_buffer),	//size_t sizeInWords,
				 drive,						//const wchar_t *drive,
				 dir,						//const wchar_t *dir,
				 fname,						//const wchar_t *fname,
				 ext);						//const wchar_t *ext


	////////////////////////////////////////////////////////////////////////////
	_wmakepath_s(tiffs_path,				//wchar_t *path,
				 _countof(tiffs_path),		//size_t sizeInWords,
				 drive,						//const wchar_t *drive,
				 dir,						//const wchar_t *dir,
				 L"*",						//const wchar_t *fname,
				 ext);						//const wchar_t *ext

	
	//Image file (with extension), without full path.
	std::wstring image_file_name = std::wstring(fname) + std::wstring(ext);

	CListOfFiles list;

    int n_files = list.dir(wpath_buffer); //Making sure that the (first) input image exists

	if (n_files <= 0)
	{
        showErrorMessageAndExit(std::wstring(L"Can't find file: ") + wpath_buffer, true);
	}

	n_files = list.dir(std::wstring(tiffs_path));

	if (n_files <= 0)
	{
        showErrorMessageAndExit(std::wstring(L"Can't find files: ") + tiffs_path, true);
	}

	int idx = list.findFileIdx(image_file_name);
	
	if (idx < 0)
	{
        showErrorMessageAndExit(L"Can't find file: " + image_file_name, true);
	}
	////////////////////////////////////////////////////////////////////////////


	Mat J;
	Mat I = myimread(wpath_buffer, IMREAD_UNCHANGED, true);

	J = processImageForDisplay(I);  //Process image for display (if needed).

	const wchar_t *wintitle = image_file_name.c_str();
	   
	mycvNamedWindow(winname, wintitle, WINDOW_AUTOSIZE | WINDOW_GUI_EXPANDED);

	IplImage tmpJ = cvIplImage(J);
	IplImage tmpI = cvIplImage(I);

	mycvShowImage2(winname, wintitle, &tmpJ, &tmpI);

	//When OpenCV is built with Qt, the namedWindow is a Qt widget.
	QWidget *qw = (QWidget*)mycvGetWindowHandle(winname);

    qw->setWindowIcon(QIcon(":/way_view-icon"));    //Set an icon for the Named Window.

	bool is_visible = qw->isVisible();

	int key = mycvWaitKey(1);	//Call waitKey for repainting image
	
	while ((is_visible) && (key != 27))	//27 is Escape key
	{
		bool do_update_image_file = false;

		if (key == (int)ARROW_KEY::LEFT)
		{
			image_file_name = list.getPrevFileName(image_file_name);

			if (image_file_name.empty())
			{
                showErrorMessageAndExit(L"Ooops getPrevFileName failed");
			}

			do_update_image_file = true;
		}
		else if (key == (int)ARROW_KEY::RIGHT)
		{
			image_file_name = list.getNextFileName(image_file_name);

			if (image_file_name.empty())
			{
                showErrorMessageAndExit(L"Ooops getNextFileName failed");
			}

			do_update_image_file = true;
		}

		//if (key > 0) fprintf(stdout, "%d\n", key);

		//F5 - Refresh
		if (key == (int)F_KEY::F5)
		{
			//When F5 is pressed, rebuild list of files, and reload image.
			//Useful in case image are added or modified in the background (when image files are updated by other process).

			n_files = list.dir(std::wstring(tiffs_path));

			if (n_files <= 0)
			{
                showErrorMessageAndExit(std::wstring(L"list.dir can't find file ") + tiffs_path);
			}

			do_update_image_file = true;
		}

		//'B' - Change "balance" preset (selection is repeated in modulo 4: B0, B1, B2, B3, B0, B1, B2, B3, B0....)
		if ((key == (int)'b') || (key == (int)'B'))
		{
			balance_select = (balance_select + 1) % 4;

			mycvSetBalanceIcon(winname, balance_select);
		}

		if (do_update_image_file)
		{
			_wmakepath_s(wpath_buffer,				//wchar_t *path,
						 _countof(wpath_buffer),	//size_t sizeInWords,
						 drive,						//const wchar_t *drive,
						 dir,						//const wchar_t *dir,
						 image_file_name.c_str(),	//const wchar_t *fname,
						 L"");						//const wchar_t *ext

			I = myimread(wpath_buffer, IMREAD_UNCHANGED);

            J = processImageForDisplay(I);  //Process image for display (if needed).

			wintitle = image_file_name.c_str();

			tmpJ = cvIplImage(J);
			tmpI = cvIplImage(I);

			mycvShowImage2(winname, wintitle, &tmpJ, &tmpI);

			//When OpenCV is built with Qt, the namedWindow is a Qt widget.
			qw = (QWidget*)mycvGetWindowHandle(winname);
            qw->setWindowIcon(QIcon(":/way_view-icon"));    //Set an icon for the Named Window.
		}

		key = mycvWaitKey(100);

		is_visible = qw->isVisible();
	}

	mycvDestroyAllWindows();
    LocalFree(argv);   // Free memory allocated for CommandLineToArgvW arguments.
}
