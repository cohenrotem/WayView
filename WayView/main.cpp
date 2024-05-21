// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "opencv2/highgui.hpp"

#include "opencv2/core/utility.hpp"

#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgcodecs/imgcodecs_c.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#undef small
#undef abs
#endif



#include <iostream>
#include <cstring>
#include <vector>

#include <locale.h>
#include <codecvt>
#include <wchar.h>
#include <iostream>
#include <cctype>
#include <limits>

//C++17 now has the std::filesystem
#include <filesystem>


#ifdef _WIN32
//https://stackoverflow.com/questions/27759754/qt-5-4-static-build-produces-unresolved-external-symbol-link-error-in-visual-s
#include "QtCore/qplugin.h"
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#include "my_window_QT.h"

using namespace cv;

#ifdef _WIN32
#define MY_UNREFERENCED_PARAMETER(p) (p)        //Assume MSVC Compiler
#else
#define MY_UNREFERENCED_PARAMETER(p) (void)(p)  //Assume GNU Compiler
#endif


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

#ifdef _WIN32
typedef wchar_t FILENAME_CHAR_T; //Use Wide characters for Windows files system.
#else
typedef char FILENAME_CHAR_T;    //Use UTF-8 narrow characters for Linux files system.
#endif


#ifndef _WIN32
//_wfopen_s is MSVC specific, the following function is a replacement for Linux.
static int _wfopen_s(FILE** f, const FILENAME_CHAR_T *file_name, const wchar_t *mode)
{
    if (f == nullptr)
    {
        return -1;  //Return some error code
    }

    //std::wstring wide_character_file_name = std::wstring{file_name};

    //Convert std::wstring to std::string. https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
    std::wstring wide_character_mode = std::wstring{mode};
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;   //Deprecated in C++17, but we are using it anyway (define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING).
    //std::string narrow_utd8_file_name = converter.to_bytes(wide_character_file_name); //Don't convert the file name - there is a issue when with Linux file system
    std::string narrow_utd8_mode = converter.to_bytes(wide_character_mode);

    //*f = fopen(narrow_utd8_file_name.c_str(), narrow_utd8_mode.c_str());

    *f = fopen(file_name, narrow_utd8_mode.c_str());

    return 0;
}

//_fseeki64 is MSVC specific, the following function is a replacement for Linux.
static int _fseeki64(FILE* f, int64_t offset, int origin)
{
    return fseeko64(f, (off64_t)offset, origin);
}

//_ftelli64 is MSVC specific, the following function is a replacement for Linux.
static int64_t _ftelli64(FILE* f)
{
    return (int64_t)ftello64(f);
}


//fread_s is MSVC specific, the following function is a replacement for Linux.
static size_t fread_s(void* buffer, size_t buffer_size, size_t element_size, size_t element_count, FILE* f)
{
    return fread(buffer, element_size, element_count, f);
}
#endif



//Read image from file with wchar_t name
//In case image has 4 color channels, assume BGRA color space, and convert from BGRA to BGR (the application doesn't support BGRA format).
static Mat myimread(const FILENAME_CHAR_T *filename, int flags = 1, bool do_create_application_when_showing_error = false)
{
	FILE *f = nullptr;
	uint64_t file_size;
	_wfopen_s(&f, filename, L"rb");

#ifdef _WIN32
    std::wstring filename_as_wstring = std::wstring(filename);
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;   //Deprecated in C++17, but we are using it anyway (define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING).
    std::wstring filename_as_wstring = converter.from_bytes(std::string(filename));  //Convert to wstring - wstring is used by showErrorMessageAndExit
#endif

	if (f == nullptr)
	{
        showErrorMessageAndExit(std::wstring(L"Error: can't open file: ") + filename_as_wstring, do_create_application_when_showing_error);
	}

	_fseeki64(f, 0, SEEK_END);
	file_size = (uint64_t)_ftelli64(f);
	_fseeki64(f, 0, SEEK_SET);

	if (file_size > (uint64_t)std::numeric_limits<int>::max())
	{
        showErrorMessageAndExit(std::wstring(L"Error: file size is too large: ") + filename_as_wstring + L", (" + std::to_wstring(file_size) + L"bytes)", do_create_application_when_showing_error);
	}

	unsigned char *raw = new unsigned char[(size_t)file_size];

	if (raw == nullptr)
	{
        showErrorMessageAndExit(L"Error: out of memory", do_create_application_when_showing_error);
	}

	//Read file to memory buffer.
	fread_s(raw, (size_t)file_size, (size_t)1, (size_t)file_size, f);

	fclose(f);


	//https://stackoverflow.com/questions/14727267/opencv-read-jpeg-image-from-buffer
	// Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
	Mat rawData(1, (int)file_size, CV_8UC1, (void*)raw);

	Mat decodedImage = imdecode(rawData, flags);

	if (decodedImage.data == nullptr)
	{
		// Error reading raw image data
        showErrorMessageAndExit(std::wstring(L"Error decoding file ") + filename_as_wstring, do_create_application_when_showing_error);
	}

	delete[] raw;

    if (decodedImage.channels() == 4)
    {
        //Convert from BGRA to BGR if input image has 4 color channels.
        cvtColor(decodedImage, decodedImage, CV_BGRA2BGR);
    }

	return decodedImage;
}



class CListOfFilePaths
{
private:
	int pathFind(const std::vector<std::filesystem::path>& vec, const std::filesystem::path pFind)
	{
		for (std::vector<std::filesystem::path>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
            //if (iter->compare(pFind) == 0) //Lets hope that in Windows the path comparison is case insensitve, and in Linux the comparison is case sensitve.
            if (std::filesystem::equivalent(*iter, pFind))   //https://stackoverflow.com/questions/28014614/why-path-comparison-is-case-sensitive-in-latest-filesystem-draft-c
			{
				return (int)std::distance(vec.begin(), iter);
			}
		}

		return -1;
	}
   
    static __inline bool iwchar_equals(const wchar_t a, const wchar_t b)
    {
        return std::tolower(a) == std::tolower(b);
    }

    //https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
    static bool iwequals(const std::wstring& a, const std::wstring& b)
    {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(), iwchar_equals);
    }


public:
	std::vector<std::filesystem::path> Q;

	CListOfFilePaths() {}

	~CListOfFilePaths() {}

	//Build vector of files matching <dir_path/*.ext_path> (example: if dir_path is C:\Tmp and ext_path is .tif, fill Q with all tif file paths in C:\Tmp folder)
	int dir(std::filesystem::path dir_path, std::filesystem::path ext_path)
	{
		Q.clear();

        //https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
        for (const auto& entry : std::filesystem::directory_iterator{dir_path})
        {
            //Note: it doen't look so efficient to itterate all files in the directory, and check for matching extension.
            //We may consider iterating the image files without creating a list from advance.
#ifdef _WIN32
            if (iwequals(entry.path().extension().wstring(), ext_path.wstring()))   //Assume Windows - case insensitive comparison
#else
            if (entry.path().extension().compare(ext_path) == 0)    //Assume Linux - case sensitive comparison
#endif
            {
                Q.push_back(entry.path());
            }
        }
		return (int)Q.size();
	}


	//Return index of pname in vector Q, return (-1) if not exist.
	int findPathIdx(std::filesystem::path pname)
	{
		int idx;

		idx = pathFind(Q, pname);

		return idx;
	}


	//Return name of previous file path in vector Q.
	std::filesystem::path getPrevFilePath(std::filesystem::path pname)
	{
		int idx;
		int len = (int)Q.size();

		idx = pathFind(Q, pname);

		if (idx == -1)
		{
			//Return empty path if fname is not is Q.
			return std::filesystem::path();
		}

		if (idx == 0)
		{
			//If fname is the first file path in the list, return the last file
			return Q[len - 1];
		}
		else
		{
			//Return previous file path name from the list
			return Q[idx - 1];
		}
	}


	//Return name of next file path in vector Q.
	std::filesystem::path getNextFilePath(std::filesystem::path pname)
	{
		int idx;
		int len = (int)Q.size();

		idx = pathFind(Q, pname);

		if (idx == -1)
		{
			//Return empty file path if fname is not is Q.
			return std::filesystem::path();
		}

		if (idx >= (len-1))
		{
			//If fname is the last file path in the list, return the first.
			return Q[0];
		}
		else
		{
			//Return next file path name from the list
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



#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    MY_UNREFERENCED_PARAMETER(hInstance);
    MY_UNREFERENCED_PARAMETER(hPrevInstance);
    MY_UNREFERENCED_PARAMETER(lpCmdLine);
    MY_UNREFERENCED_PARAMETER(nCmdShow);

    //https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw?redirectedfrom=MSDN
    //Convert command line arguments to argc and wargv
    ////////////////////////////////////////////////////////////////////////////
    wchar_t **wargv;
    int argc;

    wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (nullptr == wargv)
    {
        showErrorMessageAndExit(L"CommandLineToArgvW failed", true);
    }    
    ////////////////////////////////////////////////////////////////////////////

    std::wstring path = (argc >= 2) ? std::wstring(wargv[1]) : std::wstring();
#else   //Linux: use standard main, and convert argv from char to wchar_t (convert to wchar_t for matching the Windows implementation that uses wchar_t).
int main(int argc, char *argv[])
{
#endif

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

#ifdef _WIN32
    std::filesystem::path image_full_path(path);    //Use wide character path in Windows
#else
    //We may need to set locale for using utf-8 encoded path characters.
    std::setlocale(LC_ALL, "");
    std::locale::global(std::locale(""));

    std::string narrow_utf8_path_string = std::string{argv[1]}; //Assume that in Linux the file path is UTF-8 encoded.

    //Convert std::string to std::wstring. https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;   //Deprecated in C++17, but we are using it anyway (define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING).
    //std::wstring path = converter.from_bytes(narrow_utf8_path_string);

    std::filesystem::path image_full_path(narrow_utf8_path_string); //Use UTF-8 narrow character path in Linux
#endif

	const wchar_t winname[] = L"I";

    int balance_select = 0;

    //C++17 now has the std::filesystem package
    //https://stackoverflow.com/questions/35530092/c-splitting-an-absolute-file-path
    //https://en.cppreference.com/w/cpp/filesystem/path/extension
    std::filesystem::path dir_path = image_full_path.parent_path();
    std::filesystem::path ext_path = image_full_path.extension();
    std::filesystem::path images_path = dir_path / L"*";   //operator/  - concatenates two paths with a directory separator //https://en.cppreference.com/w/cpp/filesystem/path/append
    images_path += ext_path;    //operator+= - concatenates two paths without introducing a directory separator	

    CListOfFilePaths paths_list;

    bool is_file_exist = std::filesystem::exists(image_full_path);

	if (!is_file_exist)
	{
        showErrorMessageAndExit(std::wstring(L"Can't find file: ") + image_full_path.wstring(), true);
	}

    int n_paths = paths_list.dir(dir_path, ext_path);

	if (n_paths <= 0)
	{
        showErrorMessageAndExit(std::wstring(L"Can't find files: ") + images_path.wstring(), true);
	}

	int idx = paths_list.findPathIdx(image_full_path);
	
	if (idx < 0)
	{
        showErrorMessageAndExit(L"Can't find file: " + image_full_path.wstring(), true);
	}


	Mat J;
#ifdef _WIN32
	Mat I = myimread(image_full_path.wstring().c_str(), IMREAD_UNCHANGED, true);
    std::wstring image_file_name = image_full_path.filename().wstring();
#else
    Mat I = myimread(image_full_path.string().c_str(), IMREAD_UNCHANGED, true); //Pass narrow UTF-8 encoded string in Linux (there is an issue using wstring with Linux file system)
    std::string image_file_name_utf8 = image_full_path.filename().string();
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;   //Deprecated in C++17, but we are using it anyway (define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING).
    std::wstring image_file_name = converter.from_bytes(image_file_name_utf8);
#endif

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
			image_full_path = paths_list.getPrevFilePath(image_full_path);

			if (image_full_path.empty())
			{
                showErrorMessageAndExit(L"Ooops getPrevFileName failed");
			}

			do_update_image_file = true;
		}
		else if (key == (int)ARROW_KEY::RIGHT)
		{
			image_full_path = paths_list.getNextFilePath(image_full_path);

			if (image_full_path.empty())
			{
                showErrorMessageAndExit(L"Ooops getNextFileName failed");
			}

			do_update_image_file = true;
		}

		//F5 - Refresh
		if (key == (int)F_KEY::F5)
		{
			//When F5 is pressed, rebuild list of files, and reload image.
			//Useful in case image are added or modified in the background (when image files are updated by other process).
			n_paths = paths_list.dir(dir_path, ext_path);
            images_path = dir_path / L"*";  //operator/  - concatenates two paths with a directory separator //https://en.cppreference.com/w/cpp/filesystem/path/append
            images_path += ext_path;        //operator+= - concatenates two paths without introducing a directory separator	

			if (n_paths <= 0)
			{
                showErrorMessageAndExit(std::wstring(L"list.dir can't find file ") + images_path.wstring());
			}

            is_file_exist = std::filesystem::exists(image_full_path);

	        if (!is_file_exist)
	        {
                showErrorMessageAndExit(std::wstring(L"Can't find file: ") + image_full_path.wstring(), true);
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
#ifdef _WIN32            
			I = myimread(image_full_path.wstring().c_str(), IMREAD_UNCHANGED);
            image_file_name = image_full_path.filename().wstring();
#else
            I = myimread(image_full_path.string().c_str(), IMREAD_UNCHANGED); //Pass narrow UTF-8 encoded string in Linux (there is an issue using wstring with Linux file system)
            image_file_name_utf8 = image_full_path.filename().string();
            image_file_name = converter.from_bytes(image_file_name_utf8);
#endif            

            J = processImageForDisplay(I);  //Process image for display (if needed).
            
			wintitle = image_file_name.c_str();

			tmpJ = cvIplImage(J);
			tmpI = cvIplImage(I);

			mycvShowImage2(winname, wintitle, &tmpJ, &tmpI);

			//When OpenCV is built with Qt, the namedWindow is a Qt widget.
			qw = (QWidget*)mycvGetWindowHandle(winname);
            qw->setWindowIcon(QIcon(":/way_view-icon"));    //Set an icon for the Named Window.
		}

		key = mycvWaitKey(1);

		is_visible = qw->isVisible();
	}

	mycvDestroyAllWindows();

#ifdef _WIN32
    LocalFree(wargv);   // Free memory allocated for CommandLineToArgvW arguments.
#endif
}
