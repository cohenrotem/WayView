//IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

// By downloading, copying, installing or using the software you agree to this license.
// If you do not agree to this license, do not download, install,
// copy or use the software.


//                          License Agreement
//               For Open Source Computer Vision Library

//Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
//Copyright (C) 2008-2010, Willow Garage Inc., all rights reserved.
//Third party copyrights are property of their respective owners.

//Redistribution and use in source and binary forms, with or without modification,
//are permitted provided that the following conditions are met:

//  * Redistribution's of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.

//  * Redistribution's in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.

//  * The name of the copyright holders may not be used to endorse or promote products
//  derived from this software without specific prior written permission.

//This software is provided by the copyright holders and contributors "as is" and
//any express or implied warranties, including, but not limited to, the implied
//warranties of merchantability and fitness for a particular purpose are disclaimed.
//In no event shall the Intel Corporation or contributors be liable for any direct,
//indirect, incidental, special, exemplary, or consequential damages
//(including, but not limited to, procurement of substitute goods or services;
//loss of use, data, or profits; or business interruption) however caused
//and on any theory of liability, whether in contract, strict liability,
//or tort (including negligence or otherwise) arising in any way out of
//the use of this software, even if advised of the possibility of such damage.

//--------------------Google Code 2010 -- Yannick Verdie--------------------//

//#include "precomp.hpp"
//#include "pch.h"
#define HAVE_QT

#if defined(HAVE_QT)

#include <memory>

#include "my_window_QT.h"

#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef HAVE_QT_OPENGL
    #if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
        #include <GL/glx.h>
    #endif
#endif




//Static and global first
static MyGuiReceiver *guiMainThread = nullptr;
static int parameterSystemC = 1;
static char* parameterSystemV[] = {(char*)""};
static bool multiThreads = false;
static int last_key = -1;
QWaitCondition mykey_pressed;
QMutex mymutexKey;
static const unsigned int threshold_zoom_img_region = 30;
//the minimum zoom value to start displaying the values in the grid
//that is also the number of pixel per grid

static MyCvWinProperties* global_control_panel = nullptr;
//end static and global

// Declaration
Qt::ConnectionType myautoBlockingConnection();

// Implementation - this allows us to do blocking whilst automatically selecting the right
// behaviour for in-thread and out-of-thread launches of cv windows. Qt strangely doesn't
// cater for this, but does for strictly queued connections.
Qt::ConnectionType myautoBlockingConnection() {
  return (QThread::currentThread() != QApplication::instance()->thread())
      ? Qt::BlockingQueuedConnection
      : Qt::DirectConnection;
}

MY_CV_IMPL CvFont mycvFontQt(const char* nameFont, int pointSize,CvScalar color,int weight,int style, int spacing)
{
    /*
    //nameFont   <- only Qt
    //CvScalar color   <- only Qt (blue_component, green_component, red\_component[, alpha_component])
    int         font_face;//<- style in Qt
    const int*  ascii;
    const int*  greek;
    const int*  cyrillic;
    float       hscale, vscale;
    float       shear;
    int         thickness;//<- weight in Qt
    float       dx;//spacing letter in Qt (0 default) in pixel
    int         line_type;//<- pointSize in Qt
    */
    CvFont f = {nameFont,color,style,NULL,NULL,NULL,0,0,0,weight,(float)spacing,pointSize};
    return f;
}


MY_CV_IMPL void mycvAddText(const CvArr* img, const char* text, CvPoint org, CvFont* font)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "putText",
        myautoBlockingConnection(),
        Q_ARG(void*, (void*) img),
        Q_ARG(QString,QString::fromUtf8(text)),
        Q_ARG(QPoint, QPoint(org.x,org.y)),
        Q_ARG(void*,(void*) font));
}


double mycvGetRatioWindow_QT(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    double result = -1;
    QMetaObject::invokeMethod(guiMainThread,
        "getRatioWindow",
        myautoBlockingConnection(),
        Q_RETURN_ARG(double, result),
        Q_ARG(QString, QString::fromWCharArray(name)));

    return result;
}

double mycvGetPropVisible_QT(const wchar_t* name) {
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    double result = 0;

    QMetaObject::invokeMethod(guiMainThread,
        "getWindowVisible",
        myautoBlockingConnection(),
        Q_RETURN_ARG(double, result),
        Q_ARG(QString, QString::fromWCharArray(name)));

    return result;
}

void mycvSetRatioWindow_QT(const wchar_t* name,double prop_value)
{

    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "setRatioWindow",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(double, prop_value));
}

double mycvGetPropWindow_QT(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    double result = -1;
    QMetaObject::invokeMethod(guiMainThread,
        "getPropWindow",
        myautoBlockingConnection(),
        Q_RETURN_ARG(double, result),
        Q_ARG(QString, QString::fromWCharArray(name)));

    return result;
}


void mycvSetPropWindow_QT(const wchar_t* name,double prop_value)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "setPropWindow",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(double, prop_value));
}

//void cv::setWindowTitle(const String& winname, const String& title)
void mysetWindowTitle(const std::string& winname, const std::string& title)
{
    if (!guiMainThread)
        //CV_Error(Error::StsNullPtr, "NULL guiReceiver (please create a window)");
		CV_Error((-27), "NULL guiReceiver (please create a window)");

    QMetaObject::invokeMethod(guiMainThread,
        "setWindowTitle",
        myautoBlockingConnection(),
        Q_ARG(QString, QString(winname.c_str())),
        Q_ARG(QString, QString(title.c_str())));
}


void mycvSetModeWindow_QT(const wchar_t* name, double prop_value)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "toggleFullScreen",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(double, prop_value));
}

CvRect mycvGetWindowRect_QT(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    CvRect result = cvRect(-1, -1, -1, -1);

    QMetaObject::invokeMethod(guiMainThread,
        "getWindowRect",
        myautoBlockingConnection(),
        Q_RETURN_ARG(CvRect, result),
        Q_ARG(QString, QString::fromWCharArray(name)));

    return result;
}

double mycvGetModeWindow_QT(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    double result = -1;

    QMetaObject::invokeMethod(guiMainThread,
        "isFullScreen",
        myautoBlockingConnection(),
        Q_RETURN_ARG(double, result),
        Q_ARG(QString, QString::fromWCharArray(name)));

    return result;
}


MY_CV_IMPL void mycvDisplayOverlay(const wchar_t* name, const char* text, int delayms)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "displayInfo",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(QString, QString(text)),
        Q_ARG(int, delayms));
}


MY_CV_IMPL void mycvSaveWindowParameters(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "saveWindowParameters",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)));
}


MY_CV_IMPL void mycvLoadWindowParameters(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "loadWindowParameters",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)));
}


MY_CV_IMPL void mycvDisplayStatusBar(const wchar_t* name, const char* text, int delayms)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "displayStatusBar",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(QString, QString(text)),
        Q_ARG(int, delayms));
}


MY_CV_IMPL int mycvWaitKey(int delay)
{
    int result = -1;

    if (!guiMainThread)
        return result;

    unsigned long delayms = delay <= 0 ? ULONG_MAX : delay; //in milliseconds

    if (multiThreads)
    {
        mymutexKey.lock();
        if (mykey_pressed.wait(&mymutexKey, delayms)) //false if timeout
        {
            result = last_key;
        }
        last_key = -1;
        mymutexKey.unlock();
    }
    else
    {
        //cannot use wait here because events will not be distributed before processEvents (the main eventLoop is broken)
        //so I create a Thread for the QTimer

        if (delay > 0)
            guiMainThread->timer->start(delay);

        //QMutex dummy;

        while (!guiMainThread->bTimeOut)
        {
            qApp->processEvents(QEventLoop::AllEvents);

            if (!guiMainThread)//when all the windows are deleted
                return result;

            mymutexKey.lock();
            if (last_key != -1)
            {
                result = last_key;
                last_key = -1;
                guiMainThread->timer->stop();
                //printf("keypressed\n");
            }
            mymutexKey.unlock();

            if (result!=-1)
            {
                break;
            }
            else
            {
                /*
    * //will not work, I broke the event loop !!!!
    dummy.lock();
    QWaitCondition waitCondition;
    waitCondition.wait(&dummy, 2);
    */

                //to decrease CPU usage
                //sleep 1 millisecond
#if defined _WIN32
                Sleep(1);
#else
                usleep(1000);
#endif
            }
        }

        guiMainThread->bTimeOut = false;
    }
    return result;
}


//Yannick Verdie
//This function is experimental and some functions (such as cvSet/getWindowProperty will not work)
//We recommend not using this function for now
MY_CV_IMPL int mycvStartLoop(int (*pt2Func)(int argc, char *argv[]), int argc, char* argv[])
{
    multiThreads = true;
    QFuture<int> future = QtConcurrent::run(pt2Func, argc, argv);
    return guiMainThread->start();
}


MY_CV_IMPL void mycvStopLoop()
{
    qApp->exit();
}


static MyCvWindow* icvFindWindowByName(QString name)
{
    MyCvWindow* window = 0;

    //This is not a very clean way to do the stuff. Indeed, QAction automatically generate toolTil (QLabel)
    //that can be grabbed here and crash the code at 'w->param_name==name'.
    foreach (QWidget* widget, QApplication::topLevelWidgets())
    {
        if (widget->isWindow() && !widget->parentWidget())//is a window without parent
        {
            MyCvWinModel* temp = (MyCvWinModel*) widget;

            if (temp->type == type_CvWindow)
            {
                MyCvWindow* w = (MyCvWindow*) temp;
                if (w->objectName() == name)
                {
                    window = w;
                    break;
                }
            }
        }
    }

    return window;
}


static MyCvBar* icvFindBarByName(QBoxLayout* layout, QString name_bar, typeBar type)
{
    if (!layout)
        return NULL;

    int stop_index = layout->layout()->count();

    for (int i = 0; i < stop_index; ++i)
    {
        MyCvBar* t = (MyCvBar*) layout->layout()->itemAt(i);

        if (t->type == type && t->name_bar == name_bar)
            return t;
    }

    return NULL;
}


static MyCvTrackbar* icvFindTrackBarByName(const wchar_t* name_trackbar, const wchar_t* name_window, QBoxLayout* layout = NULL)
{


    QString nameQt = QString::fromWCharArray(name_trackbar);
    QString nameWinQt = QString::fromWCharArray(name_window);

    if (nameWinQt.isEmpty() && global_control_panel) //window name is null and we have a control panel
        layout = global_control_panel->myLayout;

    if (!layout)
    {
        QPointer<MyCvWindow> w = icvFindWindowByName(nameWinQt);

        if (!w)
            CV_Error(CV_StsNullPtr, "NULL window handler");

        if (w->param_gui_mode == CV_GUI_NORMAL)
            return (MyCvTrackbar*) icvFindBarByName(w->myBarLayout, nameQt, type_CvTrackbar);

        if (w->param_gui_mode == CV_GUI_EXPANDED)
        {
            MyCvBar* result = icvFindBarByName(w->myBarLayout, nameQt, type_CvTrackbar);

            if (result)
                return (MyCvTrackbar*) result;

            return (MyCvTrackbar*) icvFindBarByName(global_control_panel->myLayout, nameQt, type_CvTrackbar);
        }

        return NULL;
    }
    else
    {
        //layout was specified
        return (MyCvTrackbar*) icvFindBarByName(layout, nameQt, type_CvTrackbar);
    }
}

/*
static MyCvButtonbar* icvFindButtonBarByName(const char* button_name, QBoxLayout* layout)
{
    QString nameQt(button_name);
    return (MyCvButtonbar*) icvFindBarByName(layout, nameQt, type_CvButtonbar);
}
*/

static int icvInitSystem(int* c, char** v)
{
    //"For any GUI application using Qt, there is precisely one QApplication object"
    if (!QApplication::instance())
    {
        new QApplication(*c, v);
        setlocale(LC_NUMERIC,"C");

        qDebug() << "init done";

#ifdef HAVE_QT_OPENGL
        qDebug() << "opengl support available";
#endif
    }

    return 0;
}


MY_CV_IMPL int mycvInitSystem(int, char**)
{
    icvInitSystem(&parameterSystemC, parameterSystemV);
    return 0;
}


//MY_CV_IMPL int cvNamedWindow(const wchar_t* name, int flags = CV_WINDOW_AUTOSIZE)

//name is used as OpenCV legacy, and title is the windows name displayed to the user.
MY_CV_IMPL int mycvNamedWindow(const wchar_t* name, const wchar_t* title, int flags)
{
    if (!guiMainThread)
        guiMainThread = new MyGuiReceiver;

    if (QThread::currentThread() != QApplication::instance()->thread()) {
        multiThreads = true;
        QMetaObject::invokeMethod(guiMainThread,
        "createWindow",
        Qt::BlockingQueuedConnection,  // block so that we can do useful stuff once we confirm it is created
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(int, flags));
     } else {
        guiMainThread->createWindow(QString::fromWCharArray(name), flags);
     }


	/*** Rotem ***/
	QPointer<MyCvWindow> w = icvFindWindowByName(QString::fromWCharArray(name));

	if (w == NULL)
	{
		CV_Error(CV_StsNullPtr, "icvFindWindowByName returned NULL");
		return -1;
	}

	w->setWindowTitle(QString::fromWCharArray(title));

    //Execute setPropWindow - needed in case image resoluton is larger than screen resolution.
    w->setPropWindow(CV_WINDOW_AUTOSIZE);

    return 1; //Dummy value - probably should return the result of the invocation.
}


MY_CV_IMPL void mycvDestroyWindow(const wchar_t* name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "destroyWindow",
        Qt::AutoConnection,  // if another thread is controlling, let it handle it without blocking ourselves here
        Q_ARG(QString, QString::fromWCharArray(name)));
}


MY_CV_IMPL void mycvDestroyAllWindows()
{
    if (!guiMainThread)
        return;
    QMetaObject::invokeMethod(guiMainThread,
        "destroyAllWindow",
        Qt::AutoConnection  // if another thread is controlling, let it handle it without blocking ourselves here
        );
}


MY_CV_IMPL void* mycvGetWindowHandle(const wchar_t* name)
{
    if (!name)
        CV_Error( CV_StsNullPtr, "NULL name string" );

    return (void*) icvFindWindowByName(QString::fromWCharArray(name));
}


MY_CV_IMPL const wchar_t* mycvGetWindowName(void* window_handle)
{
    if( !window_handle )
        CV_Error( CV_StsNullPtr, "NULL window handler" );

	MyCvWindow *w = (MyCvWindow*)window_handle;

	w->objectName().toWCharArray(w->wname);

    //return ((MyCvWindow*)window_handle)->objectName().toLatin1().data();
	return w->wname;
}


MY_CV_IMPL void mycvMoveWindow(const wchar_t* name, int x, int y)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );
    QMetaObject::invokeMethod(guiMainThread,
        "moveWindow",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(int, x),
        Q_ARG(int, y));
}

MY_CV_IMPL void mycvResizeWindow(const wchar_t* name, int width, int height)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );
    QMetaObject::invokeMethod(guiMainThread,
        "resizeWindow",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name)),
        Q_ARG(int, width),
        Q_ARG(int, height));
}


MY_CV_IMPL int mycvCreateTrackbar2(const wchar_t* name_bar, const wchar_t* window_name, int* val, int count, CvTrackbarCallback2 on_notify, void* userdata)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "addSlider2",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name_bar)),
        Q_ARG(QString, QString::fromWCharArray(window_name)),
        Q_ARG(void*, (void*)val),
        Q_ARG(int, count),
        Q_ARG(void*, (void*)on_notify),
        Q_ARG(void*, (void*)userdata));

    return 1; //dummy value
}


MY_CV_IMPL int mycvStartWindowThread()
{
    return 0;
}


MY_CV_IMPL int mycvCreateTrackbar(const wchar_t* name_bar, const wchar_t* window_name, int* value, int count, CvTrackbarCallback on_change)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "addSlider",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(name_bar)),
        Q_ARG(QString, QString::fromWCharArray(window_name)),
        Q_ARG(void*, (void*)value),
        Q_ARG(int, count),
        Q_ARG(void*, (void*)on_change));

    return 1; //dummy value
}


MY_CV_IMPL int mycvCreateButton(const char* button_name, CvButtonCallback on_change, void* userdata, int button_type, int initial_button_state)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    if (initial_button_state < 0 || initial_button_state > 1)
        return 0;

    QMetaObject::invokeMethod(guiMainThread,
        "addButton",
        myautoBlockingConnection(),
        Q_ARG(QString, QString(button_name)),
        Q_ARG(int,  button_type),
        Q_ARG(int, initial_button_state),
        Q_ARG(void*, (void*)on_change),
        Q_ARG(void*, userdata));

    return 1;//dummy value
}


MY_CV_IMPL int mycvGetTrackbarPos(const wchar_t* name_bar, const wchar_t* window_name)
{
    int result = -1;

    QPointer<MyCvTrackbar> t = icvFindTrackBarByName(name_bar, window_name);

    if (t)
        result = t->slider->value();

    return result;
}


MY_CV_IMPL void mycvSetTrackbarPos(const wchar_t* name_bar, const wchar_t* window_name, int pos)
{
    QPointer<MyCvTrackbar> t = icvFindTrackBarByName(name_bar, window_name);

    if (t)
        t->slider->setValue(pos);
}


MY_CV_IMPL void mycvSetTrackbarMax(const wchar_t* name_bar, const wchar_t* window_name, int maxval)
{
    QPointer<MyCvTrackbar> t = icvFindTrackBarByName(name_bar, window_name);
    if (t)
    {
        t->slider->setMaximum(maxval);
    }
}


MY_CV_IMPL void mycvSetTrackbarMin(const wchar_t* name_bar, const wchar_t* window_name, int minval)
{
    QPointer<MyCvTrackbar> t = icvFindTrackBarByName(name_bar, window_name);
    if (t)
    {
        t->slider->setMinimum(minval);
    }
}


/* assign callback for mouse events */
MY_CV_IMPL void mycvSetMouseCallback(const wchar_t* window_name, CvMouseCallback on_mouse, void* param)
{
    QPointer<MyCvWindow> w = icvFindWindowByName(QString::fromWCharArray(window_name));

    if (!w)
        CV_Error(CV_StsNullPtr, "NULL window handler");

    w->setMouseCallBack(on_mouse, param);

}


MY_CV_IMPL void mycvShowImage(const wchar_t* name, const wchar_t* title, const CvArr* arr)
{
    if (!guiMainThread)
        guiMainThread = new MyGuiReceiver;
    if (QThread::currentThread() != QApplication::instance()->thread()) {
        multiThreads = true;
        QMetaObject::invokeMethod(guiMainThread,
            "showImage",
             myautoBlockingConnection(),
             Q_ARG(QString, QString::fromWCharArray(name)),
			 Q_ARG(QString, QString::fromWCharArray(title)),
             Q_ARG(void*, (void*)arr)
        );
     } else {
        guiMainThread->showImage(QString::fromWCharArray(name), QString::fromWCharArray(title), (void*)arr);
     }
}


//Use two images: 
//1. arr - image for display (e.g 8 bits per color channel)
//2. arr2 - original image (e.g 16 bits per color channel) - pixel value in (x,y) is from arr2
//name is used as OpenCV legacy, and title is the windows name displayed to the user.
MY_CV_IMPL void mycvShowImage2(const wchar_t* name, const wchar_t* title, const CvArr* arr, const CvArr* arr2)
{
	if (!guiMainThread)
		guiMainThread = new MyGuiReceiver;

	if (QThread::currentThread() != QApplication::instance()->thread()) {
		multiThreads = true;
		QMetaObject::invokeMethod(guiMainThread,
			"showImage2",
			myautoBlockingConnection(),
			Q_ARG(QString, QString::fromWCharArray(name)),
			Q_ARG(QString, QString::fromWCharArray(title)),
			Q_ARG(void*, (void*)arr),
			Q_ARG(void*, (void*)arr2)
		);
	}
	else {
		guiMainThread->showImage2(QString::fromWCharArray(name), QString::fromWCharArray(title), (void*)arr, (void*)arr2);
	}
}



/*** Rotem ***/
//////////////////////////////////////////////////////////////////////////
//Set icon of selected "balance" in the toolbar.
//Valid values: 0, 1, 2, 3
//Value of -1, disable the icon (makes it gray).
MY_CV_IMPL void mycvSetBalanceIcon(const wchar_t* name, int balance_select)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(QString::fromWCharArray(name));

	if (!w)
	{
		CV_Error(CV_StsNullPtr, "NULL window handler");
	}

	switch (balance_select)
	{
		case -1:
			w->vect_QActions[9]->setDisabled(true);
			break;

		case 0:
			w->vect_QActions[9]->setIcon(QIcon(":/B0-icon"));
			break;

		case 1:
			w->vect_QActions[9]->setIcon(QIcon(":/B1-icon"));
			break;

		case 2:
			w->vect_QActions[9]->setIcon(QIcon(":/B2-icon"));
			break;

		case 3:
			w->vect_QActions[9]->setIcon(QIcon(":/B3-icon"));
			break;

		default:
			break;
	}

	if (balance_select >= 0)
	{
		w->vect_QActions[9]->setDisabled(false);
	}
	
	w->vect_QActions[9]->setIconVisibleInMenu(true);
}
//////////////////////////////////////////////////////////////////////////
/*** Rotem ***/



#ifdef HAVE_QT_OPENGL

MY_CV_IMPL void mycvSetOpenGlDrawCallback(const wchar_t* window_name, CvOpenGlDrawCallback callback, void* userdata)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "setOpenGlDrawCallback",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(window_name)),
        Q_ARG(void*, (void*)callback),
        Q_ARG(void*, userdata));
}


MY_CV_IMPL void mycvSetOpenGlContext(const wchar_t* window_name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "setOpenGlContext",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(window_name)));
}


MY_CV_IMPL void mycvUpdateWindow(const wchar_t* window_name)
{
    if (!guiMainThread)
        CV_Error( CV_StsNullPtr, "NULL guiReceiver (please create a window)" );

    QMetaObject::invokeMethod(guiMainThread,
        "updateWindow",
        myautoBlockingConnection(),
        Q_ARG(QString, QString::fromWCharArray(window_name)));
}

#endif


double mycvGetOpenGlProp_QT(const wchar_t* name)
{
    double result = -1;

    if (guiMainThread)
    {
        QMetaObject::invokeMethod(guiMainThread,
            "isOpenGl",
            myautoBlockingConnection(),
            Q_RETURN_ARG(double, result),
            Q_ARG(QString, QString::fromWCharArray(name)));
    }

    return result;
}


//////////////////////////////////////////////////////
// MyGuiReceiver

MyGuiReceiver::MyGuiReceiver() : bTimeOut(false), nb_windows(0)
{
	doesExternalQAppExist = (QApplication::instance() != 0);
	icvInitSystem(&parameterSystemC, parameterSystemV);

	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));
	timer->setSingleShot(true);
	if (doesExternalQAppExist) {
		moveToThread(QApplication::instance()->thread());
	}
}


void MyGuiReceiver::isLastWindow()
{
	if (--nb_windows <= 0)
	{
		delete guiMainThread;//delete global_control_panel too
		guiMainThread = NULL;

		if (!doesExternalQAppExist)
		{
			qApp->quit();
		}
	}
}


MyGuiReceiver::~MyGuiReceiver()
{
	if (global_control_panel)
	{
		delete global_control_panel;
		global_control_panel = NULL;
	}
}


void MyGuiReceiver::putText(void* arr, QString text, QPoint org, void* arg2)
{
    CV_Assert(arr);

    CvMat* mat, stub;
    mat = cvGetMat(arr, &stub);

    int nbChannelOriginImage = cvGetElemType(mat);
    if (nbChannelOriginImage != CV_8UC3) return; //for now, font works only with 8UC3

    QImage qimg(mat->data.ptr, mat->cols, mat->rows, mat->step, QImage::Format_RGB888);

    CvFont* font = (CvFont*)arg2;

    QPainter qp(&qimg);
    if (font)
    {
        QFont f(font->nameFont, font->line_type/*PointSize*/, font->thickness/*weight*/);
        f.setStyle((QFont::Style) font->font_face/*style*/);
        f.setLetterSpacing(QFont::AbsoluteSpacing, font->dx/*spacing*/);
        //cvScalar(blue_component, green_component, red_component[, alpha_component])
        //Qt map non-transparent to 0xFF and transparent to 0
        //OpenCV scalar is the reverse, so 255-font->color.val[3]
        qp.setPen(QColor((int)std::round(font->color.val[0]), (int)std::round(font->color.val[1]), (int)std::round(font->color.val[2]), 255 - (int)std::round(font->color.val[3])));
        qp.setFont(f);
    }
    qp.drawText(org, text);
    qp.end();
}


void MyGuiReceiver::saveWindowParameters(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->writeSettings();
}


void MyGuiReceiver::loadWindowParameters(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->readSettings();
}


double MyGuiReceiver::getRatioWindow(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return -1;

	return w->getRatio();
}


void MyGuiReceiver::setRatioWindow(QString name, double arg2)
{
	//QPointer<MyCvWindow> w = icvFindWindowByName(name.toLatin1().data());
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return;

	int flags = (int)arg2;

	w->setRatio(flags);
}


double MyGuiReceiver::getPropWindow(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return -1;

	return (double)w->getPropWindow();
}

double MyGuiReceiver::getWindowVisible(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return 0;

	return (double)w->isVisible();
}


void MyGuiReceiver::setPropWindow(QString name, double arg2)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return;

	int flags = (int)arg2;

	w->setPropWindow(flags);
}

void MyGuiReceiver::setWindowTitle(QString name, QString title)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
	{
		wchar_t *wname = new wchar_t[name.length() + 1];
		wchar_t *wtitle = new wchar_t[title.length() + 1];
		name.toWCharArray(wname);
		title.toWCharArray(wtitle);

		//mycvNamedWindow(name.toLatin1().data());
		mycvNamedWindow(wname, wtitle);
		w = icvFindWindowByName(name);

		delete[] wname;
		delete[] wtitle;
	}

	if (!w)
		return;

	w->setWindowTitle(title);
}

CvRect MyGuiReceiver::getWindowRect(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return cvRect(-1, -1, -1, -1);

	return w->getWindowRect();
}

double MyGuiReceiver::isFullScreen(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return -1;

	return w->isFullScreen() ? CV_WINDOW_FULLSCREEN : CV_WINDOW_NORMAL;
}


void MyGuiReceiver::toggleFullScreen(QString name, double arg2)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w)
		return;

	int flags = (int)arg2;

	w->toggleFullScreen(flags);
}


void MyGuiReceiver::createWindow(QString name, int flags)
{
	if (!qApp)
		CV_Error(CV_StsNullPtr, "NULL session handler");

	// Check the name in the storage
	//if (icvFindWindowByName(name.toLatin1().data()))
	if (icvFindWindowByName(name))
	{
		return;
	}

	nb_windows++;
	new MyCvWindow(name, flags);
	mycvWaitKey(1);
}


void MyGuiReceiver::timeOut()
{
	bTimeOut = true;
}


void MyGuiReceiver::displayInfo(QString name, QString text, int delayms)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->displayInfo(text, delayms);
}


void MyGuiReceiver::displayStatusBar(QString name, QString text, int delayms)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->displayStatusBar(text, delayms);
}


void MyGuiReceiver::showImage(QString name, QString title, void* arr)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w) //as observed in the previous implementation (W32, GTK or Carbon), create a new window is the pointer returned is null
	{
		wchar_t *wname = new wchar_t[name.length() + 1];
		wchar_t *wtitle = new wchar_t[title.length() + 1];
		name.toWCharArray(wname);
		title.toWCharArray(wtitle);

		//mycvNamedWindow(name.toLatin1().data());
		mycvNamedWindow(wname, wtitle);
		w = icvFindWindowByName(name);

		delete[] wname;
		delete[] wtitle;
	}

	if (!w || !arr)
		return; // keep silence here.

	if (w->isOpenGl())
	{
		CvMat* mat, stub;

		mat = cvGetMat(arr, &stub);

		cv::Mat im = cv::cvarrToMat(mat);
		cv::imshow(name.toUtf8().data(), im);
	}
	else
	{
		w->updateImage(arr);
	}

	if (w->isHidden())
		w->show();
}



void MyGuiReceiver::showImage2(QString name, QString title, void* arr, void* arr2)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (!w) //as observed in the previous implementation (W32, GTK or Carbon), create a new window is the pointer returned is null
	{
		wchar_t *wname = new wchar_t[name.length() + 1];
		wchar_t *wtitle = new wchar_t[title.length() + 1];
		name.toWCharArray(wname);
		title.toWCharArray(wtitle);


		//mycvNamedWindow(name.toLatin1().data());
		mycvNamedWindow(wname, wtitle);

		w = icvFindWindowByName(name);

		delete[] wname;
		delete[] wtitle;
	}

	if (!w || !arr || !arr2)
		return; // keep silence here.

	//if (w->isOpenGl())  /*** Rotem ***/
	//{
	//	CvMat* mat, stub;
    //
	//	mat = cvGetMat(arr, &stub);
    //
	//	cv::Mat im = cv::cvarrToMat(mat);
	//	cv::imshow(name.toUtf8().data(), im);
	//}
	//else
	{
		//w->updateImage(arr);
		w->updateImage2(arr, arr2);	/*** Rotem ***/
		w->setWindowTitle(title);	/*** Rotem ***/

        //Execute setPropWindow - needed in case image resoluton is larger than screen resolution.
        w->setPropWindow(CV_WINDOW_AUTOSIZE);	/*** Rotem ***/
	}

	if (w->isHidden())
		w->show();
}




void MyGuiReceiver::destroyWindow(QString name)
{

	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
	{
		w->close();

		//in not-multiThreads mode, looks like the window is hidden but not deleted
		//so I do it manually
		//otherwise QApplication do it for me if the exec command was executed (in multiThread mode)
		if (!multiThreads)
			delete w;
	}
}


void MyGuiReceiver::destroyAllWindow()
{
	if (!qApp)
		CV_Error(CV_StsNullPtr, "NULL session handler");

	if (multiThreads)
	{
		// WARNING: this could even close windows from an external parent app
		//#TODO check externalQAppExists and in case it does, close windows carefully,
		//      i.e. apply the className-check from below...
		qApp->closeAllWindows();
	}
	else
	{
		bool isWidgetDeleted = true;
		while (isWidgetDeleted)
		{
			isWidgetDeleted = false;
			QWidgetList list = QApplication::topLevelWidgets();
			for (int i = 0; i < list.count(); i++)
			{
				QObject *obj = list.at(i);
				if (obj->metaObject()->className() == QString("MyCvWindow"))
				{
					delete obj;
					isWidgetDeleted = true;
					break;
				}
			}
		}
	}
}


void MyGuiReceiver::moveWindow(QString name, int x, int y)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->move(x, y);
}


void MyGuiReceiver::resizeWindow(QString name, int width, int height)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
	{
		w->showNormal();
		w->setViewportSize(QSize(width, height));
	}
}


void MyGuiReceiver::enablePropertiesButtonEachWindow()
{
	//For each window, enable window property button
	foreach(QWidget* widget, QApplication::topLevelWidgets())
	{
		if (widget->isWindow() && !widget->parentWidget()) //is a window without parent
		{
			MyCvWinModel* temp = (MyCvWinModel*)widget;
			if (temp->type == type_CvWindow)
			{
				MyCvWindow* w = (MyCvWindow*)widget;

				//active window properties button
				w->enablePropertiesButton();
			}
		}
	}
}


void MyGuiReceiver::addButton(QString button_name, int button_type, int initial_button_state, void* on_change, void* userdata)
{
	if (!global_control_panel)
		return;

	QPointer<MyCvButtonbar> b;

	if (global_control_panel->myLayout->count() == 0) //if that is the first button attach to the control panel, create a new button bar
	{
		b = MyCvWindow::createButtonBar(button_name); //the bar has the name of the first button attached to it
		enablePropertiesButtonEachWindow();

	}
	else
	{
		MyCvBar* lastbar = (MyCvBar*)global_control_panel->myLayout->itemAt(global_control_panel->myLayout->count() - 1);

		// if last bar is a trackbar or the user requests a new buttonbar, create a new buttonbar
		// else, attach to the current bar
		if (lastbar->type == type_CvTrackbar || cv::QT_NEW_BUTTONBAR & button_type)
			b = MyCvWindow::createButtonBar(button_name); //the bar has the name of the first button attached to it
		else
			b = (MyCvButtonbar*)lastbar;

	}

	// unset buttonbar flag
	button_type = button_type & ~cv::QT_NEW_BUTTONBAR;

	b->addButton(button_name, (CvButtonCallback)on_change, userdata, button_type, initial_button_state);
}


void MyGuiReceiver::addSlider2(QString bar_name, QString window_name, void* value, int count, void* on_change, void *userdata)
{
	QBoxLayout *layout = NULL;
	QPointer<MyCvWindow> w;

	if (!window_name.isEmpty())
	{
		w = icvFindWindowByName(window_name);

		if (!w)
			return;
	}
	else
	{
		if (global_control_panel)
			layout = global_control_panel->myLayout;
	}

	wchar_t *wbar_name = new wchar_t[bar_name.length() + 1];
	wchar_t *wwindow_name = new wchar_t[window_name.length() + 1];

	bar_name.toWCharArray(wbar_name);
	window_name.toWCharArray(wwindow_name);

	//QPointer<MyCvTrackbar> t = icvFindTrackBarByName(bar_name.toLatin1().data(), window_name.toLatin1().data(), layout);
	QPointer<MyCvTrackbar> t = icvFindTrackBarByName(wbar_name, wwindow_name, layout);

	delete[] wbar_name;
	delete[] wwindow_name;


	if (t) //trackbar exists
		return;

	if (!value)
		CV_Error(CV_StsNullPtr, "NULL value pointer");

	if (count <= 0) //count is the max value of the slider, so must be bigger than 0
		CV_Error(CV_StsNullPtr, "Max value of the slider must be bigger than 0");

	MyCvWindow::addSlider2(w, bar_name, (int*)value, count, (CvTrackbarCallback2)on_change, userdata);
}


void MyGuiReceiver::addSlider(QString bar_name, QString window_name, void* value, int count, void* on_change)
{
	QBoxLayout *layout = NULL;
	QPointer<MyCvWindow> w;

	if (!window_name.isEmpty())
	{
		w = icvFindWindowByName(window_name);

		if (!w)
			return;
	}
	else
	{
		if (global_control_panel)
			layout = global_control_panel->myLayout;
	}

	wchar_t *wbar_name = new wchar_t[bar_name.length() + 1];
	wchar_t *wwindow_name = new wchar_t[window_name.length() + 1];

	bar_name.toWCharArray(wbar_name);
	window_name.toWCharArray(wwindow_name);

	QPointer<MyCvTrackbar> t = icvFindTrackBarByName(wbar_name, wwindow_name, layout);

	delete[] wbar_name;
	delete[] wwindow_name;


	if (t) //trackbar exists
		return;

	if (!value)
		CV_Error(CV_StsNullPtr, "NULL value pointer");

	if (count <= 0) //count is the max value of the slider, so must be bigger than 0
		CV_Error(CV_StsNullPtr, "Max value of the slider must be bigger than 0");

	MyCvWindow::addSlider(w, bar_name, (int*)value, count, (CvTrackbarCallback)on_change);
}


int MyGuiReceiver::start()
{
	return qApp->exec();
}


void MyGuiReceiver::setOpenGlDrawCallback(QString name, void* callback, void* userdata)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->setOpenGlDrawCallback((CvOpenGlDrawCallback)callback, userdata);
}

void MyGuiReceiver::setOpenGlContext(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->makeCurrentOpenGlContext();
}

void MyGuiReceiver::updateWindow(QString name)
{
	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		w->updateGl();
}

double MyGuiReceiver::isOpenGl(QString name)
{
	double result = -1;

	QPointer<MyCvWindow> w = icvFindWindowByName(name);

	if (w)
		result = (double)w->isOpenGl();

	return result;
}


/*** Rotem ***/
////////////////////////////////////////////////////////////////////////////
//void MyGuiReceiver::onWindowStateChanged(Qt::WindowState state)
//{
//    if (state == Qt::WindowMaximized) 
//    {
//        // Maximize event handling
//        qDebug() << "Window Maximized!";
//        // You can perform actions here when the window is maximized
//    }
//}

//void MyGuiReceiver::changeEvent(QEvent *event)
//{
//    qDebug() << "Window Maximized!";
//}
////////////////////////////////////////////////////////////////////////////
/*** Rotem ***/


//////////////////////////////////////////////////////
// MyCvTrackbar


MyCvTrackbar::MyCvTrackbar(MyCvWindow* arg, QString name, int* value, int _count, CvTrackbarCallback2 on_change, void* data)
{
    callback = NULL;
    callback2 = on_change;
    userdata = data;

    create(arg, name, value, _count);
}


MyCvTrackbar::MyCvTrackbar(MyCvWindow* arg, QString name, int* value, int _count, CvTrackbarCallback on_change)
{
    callback = on_change;
    callback2 = NULL;
    userdata = NULL;

    create(arg, name, value, _count);
}


void MyCvTrackbar::create(MyCvWindow* arg, QString name, int* value, int _count)
{
    type = type_CvTrackbar;
    myparent = arg;
    name_bar = name;
    setObjectName(name_bar);
    dataSlider = value;

    slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setMinimum(0);
    slider->setMaximum(_count);
    slider->setPageStep(5);
    slider->setValue(*value);
    slider->setTickPosition(QSlider::TicksBelow);


    //Change style of the Slider
    //slider->setStyleSheet(str_Trackbar_css);

    QFile qss(":/stylesheet-trackbar");
    if (qss.open(QFile::ReadOnly))
    {
        slider->setStyleSheet(QLatin1String(qss.readAll()));
        qss.close();
    }


    //this next line does not work if we change the style with a stylesheet, why ? (bug in QT ?)
    //slider->setTickPosition(QSlider::TicksBelow);
    label = new QPushButton;
    label->setFlat(true);
    setLabel(slider->value());


    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(update(int)));

    QObject::connect(label, SIGNAL(clicked()), this, SLOT(createDialog()));

    //label->setStyleSheet("QPushButton:disabled {color: black}");

    addWidget(label, Qt::AlignLeft);//name + value
    addWidget(slider, Qt::AlignCenter);//slider
}


void MyCvTrackbar::createDialog()
{
    bool ok = false;

    //crash if I access the values directly and give them to QInputDialog, so do a copy first.
    int value = slider->value();
    int step = slider->singleStep();
    int min = slider->minimum();
    int max = slider->maximum();

    int i =
#if QT_VERSION >= 0x040500
        QInputDialog::getInt
#else
        QInputDialog::getInteger
#endif
        (this->parentWidget(),
        tr("Slider %1").arg(name_bar),
        tr("New value:"),
        value,
        min,
        max,
        step,
        &ok);

    if (ok)
        slider->setValue(i);
}


void MyCvTrackbar::update(int myvalue)
{
    setLabel(myvalue);

    *dataSlider = myvalue;
    if (callback)
    {
        callback(myvalue);
        return;
    }

    if (callback2)
    {
        callback2(myvalue, userdata);
        return;
    }
}


void MyCvTrackbar::setLabel(int myvalue)
{
    QString nameNormalized = name_bar.leftJustified( 10, ' ', false );
    QString valueMaximum = QString("%1").arg(slider->maximum());
    QString str = QString("%1 (%2/%3)").arg(nameNormalized).arg(myvalue,valueMaximum.length(),10,QChar('0')).arg(valueMaximum);
    label->setText(str);
}


//////////////////////////////////////////////////////
// MyCvButtonbar


//here MyCvButtonbar class
MyCvButtonbar::MyCvButtonbar(QWidget* arg,  QString arg2)
{
    type = type_CvButtonbar;
    myparent = arg;
    name_bar = arg2;
    setObjectName(name_bar);

    group_button = new QButtonGroup(this);
}


void MyCvButtonbar::setLabel()
{
    QString nameNormalized = name_bar.leftJustified(10, ' ', true);
    label->setText(nameNormalized);
}


void MyCvButtonbar::addButton(QString name, CvButtonCallback call, void* userdata,  int button_type, int initial_button_state)
{
    QString button_name = name;

    if (button_name == "")
        button_name = tr("button %1").arg(this->count());

    QPointer<QAbstractButton> button;

    if (button_type == CV_PUSH_BUTTON)
        button = (QAbstractButton*) new MyCvPushButton(this, button_name,call, userdata);

    if (button_type == CV_CHECKBOX)
        button = (QAbstractButton*) new MyCvCheckBox(this, button_name,call, userdata, initial_button_state);

    if (button_type == CV_RADIOBOX)
    {
        button = (QAbstractButton*) new MyCvRadioButton(this, button_name,call, userdata, initial_button_state);
        group_button->addButton(button);
    }

    if (button)
    {
        if (button_type == CV_PUSH_BUTTON)
            QObject::connect(button, SIGNAL(clicked(bool)), button, SLOT(callCallBack(bool)));
        else
            QObject::connect(button, SIGNAL(toggled(bool)), button, SLOT(callCallBack(bool)));

        addWidget(button, Qt::AlignCenter);
    }
}


//////////////////////////////////////////////////////
// Buttons


//buttons here
MyCvPushButton::MyCvPushButton(MyCvButtonbar* arg1, QString arg2, CvButtonCallback arg3, void* arg4)
{
    myparent = arg1;
    button_name = arg2;
    callback = arg3;
    userdata = arg4;

    setObjectName(button_name);
    setText(button_name);

    if (isChecked())
        callCallBack(true);
}


void MyCvPushButton::callCallBack(bool checked)
{
    if (callback)
        callback(checked, userdata);
}


MyCvCheckBox::MyCvCheckBox(MyCvButtonbar* arg1, QString arg2, CvButtonCallback arg3, void* arg4, int initial_button_state)
{
    myparent = arg1;
    button_name = arg2;
    callback = arg3;
    userdata = arg4;

    setObjectName(button_name);
    setCheckState((initial_button_state == 1 ? Qt::Checked : Qt::Unchecked));
    setText(button_name);

    if (isChecked())
        callCallBack(true);
}


void MyCvCheckBox::callCallBack(bool checked)
{
    if (callback)
        callback(checked, userdata);
}


MyCvRadioButton::MyCvRadioButton(MyCvButtonbar* arg1, QString arg2, CvButtonCallback arg3, void* arg4, int initial_button_state)
{
    myparent = arg1;
    button_name = arg2;
    callback = arg3;
    userdata = arg4;

    setObjectName(button_name);
    setChecked(initial_button_state);
    setText(button_name);

    if (isChecked())
        callCallBack(true);
}

void MyCvRadioButton::callCallBack(bool checked)
{
    if (callback)
        callback(checked, userdata);
}


//////////////////////////////////////////////////////
// MyCvWinProperties


//here MyCvWinProperties class
MyCvWinProperties::MyCvWinProperties(QString name_paraWindow, QObject* /*parent*/)
{
    //setParent(parent);
    type = type_CvWinProperties;
    setWindowFlags(Qt::Tool);
    setContentsMargins(0, 0, 0, 0);
    setWindowTitle(name_paraWindow);
    setObjectName(name_paraWindow);
    resize(100, 50);

    myLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    myLayout->setObjectName(QString::fromUtf8("boxLayout"));
    myLayout->setContentsMargins(0, 0, 0, 0);
    myLayout->setSpacing(0);
    myLayout->setMargin(0);
    myLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(myLayout);

    hide();
}


void MyCvWinProperties::closeEvent(QCloseEvent* e)
{
    e->accept(); //intersept the close event (not sure I really need it)
    //an hide event is also sent. I will intercept it and do some processing
}


void MyCvWinProperties::showEvent(QShowEvent* evnt)
{
    //why -1,-1 ?: do this trick because the first time the code is run,
    //no value pos was saved so we let Qt move the window in the middle of its parent (event ignored).
    //then hide will save the last position and thus, we want to retrieve it (event accepted).
    QPoint mypos(-1, -1);
    QSettings settings("OpenCV2", objectName());
    mypos = settings.value("pos", mypos).toPoint();

    if (mypos.x() >= 0)
    {
        move(mypos);
        evnt->accept();
    }
    else
    {
        evnt->ignore();
    }
}


void MyCvWinProperties::hideEvent(QHideEvent* evnt)
{
    QSettings settings("OpenCV2", objectName());
    settings.setValue("pos", pos()); //there is an offset of 6 pixels (so the window's position is wrong -- why ?)
    evnt->accept();
}


MyCvWinProperties::~MyCvWinProperties()
{
    //clear the setting pos
    QSettings settings("OpenCV2", objectName());
    settings.remove("pos");
}


//////////////////////////////////////////////////////
// MyCvWindow


MyCvWindow::MyCvWindow(QString name, int arg2)
{
	wname = new wchar_t[4096];	//Assume no one is crease enough to give window name longer then 4096 characters. 

    type = type_CvWindow;

    param_flags = arg2 & 0x0000000F;
    param_gui_mode = arg2 & 0x000000F0;
    param_ratio_mode =  arg2 & 0x00000F00;

    //setAttribute(Qt::WA_DeleteOnClose); //in other case, does not release memory
    setContentsMargins(0, 0, 0, 0);
    setWindowTitle(name);
    setObjectName(name);

    setFocus( Qt::PopupFocusReason ); //#1695 arrow keys are not received without the explicit focus

    resize(400, 300);
    setMinimumSize(1, 1);

    //1: create control panel
    if (!global_control_panel)
        global_control_panel = createParameterWindow();

    //2: Layouts
    createBarLayout();
    createGlobalLayout();

    //3: my view
#ifndef HAVE_QT_OPENGL
    if (arg2 & CV_WINDOW_OPENGL)
        CV_Error( CV_OpenGlNotSupported, "Library was built without OpenGL support" );
    mode_display = CV_MODE_NORMAL;
#else
    mode_display = arg2 & CV_WINDOW_OPENGL ? CV_MODE_OPENGL : CV_MODE_NORMAL;
    if (mode_display == CV_MODE_OPENGL)
        param_gui_mode = CV_GUI_NORMAL;
#endif
    createView();

    //4: shortcuts and actions
    //5: toolBar and statusbar
    if (param_gui_mode == CV_GUI_EXPANDED)
    {
        createActions();
        createShortcuts();

        createToolBar();
        createStatusBar();
    }

    //Now attach everything
    if (myToolBar)
        myGlobalLayout->addWidget(myToolBar, Qt::AlignCenter);

    myGlobalLayout->addWidget(myView->getWidget(), Qt::AlignCenter);

    myGlobalLayout->addLayout(myBarLayout, Qt::AlignCenter);

    if (myStatusBar)
        myGlobalLayout->addWidget(myStatusBar, Qt::AlignCenter);

    setLayout(myGlobalLayout);
    show();
}


MyCvWindow::~MyCvWindow()
{
	if (wname != NULL)
	{
		delete wname;
		wname = NULL;
	}

    if (guiMainThread)
        guiMainThread->isLastWindow();
}


void MyCvWindow::setMouseCallBack(CvMouseCallback callback, void* param)
{
    myView->setMouseCallBack(callback, param);
}


void MyCvWindow::writeSettings()
{
    //organisation and application's name
    QSettings settings("OpenCV2", QFileInfo(QApplication::applicationFilePath()).fileName());

    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("mode_resize" ,param_flags);
    settings.setValue("mode_gui", param_gui_mode);

    myView->writeSettings(settings);

    icvSaveTrackbars(&settings);

    if (global_control_panel)
    {
        icvSaveControlPanel();
        settings.setValue("posPanel", global_control_panel->pos());
    }
}



//TODO: load CV_GUI flag (done) and act accordingly (create win property if needed and attach trackbars)
void MyCvWindow::readSettings()
{
    //organisation and application's name
    QSettings settings("OpenCV2", QFileInfo(QApplication::applicationFilePath()).fileName());

    QPoint _pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize _size = settings.value("size", QSize(400, 400)).toSize();

    param_flags = settings.value("mode_resize", param_flags).toInt();
    param_gui_mode = settings.value("mode_gui", param_gui_mode).toInt();

    param_flags = settings.value("mode_resize", param_flags).toInt();

    myView->readSettings(settings);

    //trackbar here
    icvLoadTrackbars(&settings);

    resize(_size);
    move(_pos);

    if (global_control_panel)
    {
        icvLoadControlPanel();
        global_control_panel->move(settings.value("posPanel", global_control_panel->pos()).toPoint());
    }
}


double MyCvWindow::getRatio()
{
    return myView->getRatio();
}


void MyCvWindow::setRatio(int flags)
{
    myView->setRatio(flags);
}

CvRect MyCvWindow::getWindowRect()
{
    QWidget* view = myView->getWidget();
    QRect local_rc = view->geometry(); // http://doc.qt.io/qt-5/application-windows.html#window-geometry
    QPoint global_pos = /*view->*/mapToGlobal(QPoint(local_rc.x(), local_rc.y()));
    return cvRect(global_pos.x(), global_pos.y(), local_rc.width(), local_rc.height());
}

int MyCvWindow::getPropWindow()
{
    return param_flags;
}


/*** Rotem ***/
//https://forum.qt.io/topic/98160/find-the-active-qscreen-of-any-qwidget/6
//https://stackoverflow.com/a/42413327/4926757
////////////////////////////////////////////////////////////////////////////
static QScreen* getActiveScreenResolution(QWidget* pWidget, int *width, int *height)
{
	QScreen* pActive = nullptr;

	while (pWidget)
	{
		auto w = pWidget->windowHandle();
		if (w != nullptr)
		{
			pActive = w->screen();
			break;
		}
		else
        {
			pWidget = pWidget->parentWidget();
        }
	}

    if (pActive != nullptr)
    {
        QRect screenGeometry = pActive->geometry();
        *width = screenGeometry.width();
        *height = screenGeometry.height();
    }
    else
    {
        *width = 0;
        *height = 0;
    }

	return pActive;
}
////////////////////////////////////////////////////////////////////////////



void MyCvWindow::setPropWindow(int flags)
{
    /*** Rotem ***/
    //Put the "nothing to do" in comment.
    ////////////////////////////////////////////////////////////////////////////
    //if (param_flags == flags) //nothing to do
    //    return;
    ////////////////////////////////////////////////////////////////////////////
    /*** Rotem ***/

    /*** Rotem ***/
    ////////////////////////////////////////////////////////////////////////////
    bool is_mat_larger_then_screenresolution = isMatLargerThanScreenResolution();
    ////////////////////////////////////////////////////////////////////////////
    /*** Rotem ***/

    switch(flags)
    {
    case CV_WINDOW_NORMAL:
        myGlobalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        param_flags = flags;

        break;

    case CV_WINDOW_AUTOSIZE:
        /*** Rotem ***/
        ////////////////////////////////////////////////////////////////////////////
        //Set QLayout::SetMinAndMaxSize is shown image is too large (if image resolution is larger than screen resolution).
        if (is_mat_larger_then_screenresolution)
        {
            myGlobalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

            //Resize the window to the size of the screen and move it to (0, 0).
            //This is an ugly workaround - I coudn't find a more elegant way.
            int screen_cols, screen_rows;
            QScreen* qscreen = getActiveScreenResolution(this, &screen_cols, &screen_rows);
            if (qscreen != nullptr)
            {
                int win_cols = screen_cols-4;
                int win_rows = screen_rows-70;
                this->setFixedSize(win_cols, win_rows);
                //int x = (screen_cols - win_cols) / 2;
                //int y = (screen_rows - win_rows) / 2;
                //this->move(x, y);
                this->move(0, 0);
            }
        }
        else
        {
            myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize);
        }
        ////////////////////////////////////////////////////////////////////////////

        //myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize);
        param_flags = flags;

        break;

    default:
        ;
    }
}

void MyCvWindow::toggleFullScreen(int flags)
{
    if (isFullScreen() && flags == CV_WINDOW_NORMAL)
    {
        showTools();
        showNormal();
        return;
    }

    if (!isFullScreen() && flags == CV_WINDOW_FULLSCREEN)
    {
        hideTools();
        showFullScreen();
        return;
    }
}


void MyCvWindow::updateImage(void* arr)
{
    myView->updateImage(arr);
}


void MyCvWindow::updateImage2(void* arr, void* arr2)
{
	myView->updateImage2(arr, arr2);
}



void MyCvWindow::displayInfo(QString text, int delayms)
{
    myView->startDisplayInfo(text, delayms);
}


void MyCvWindow::displayStatusBar(QString text, int delayms)
{
    if (myStatusBar)
        myStatusBar->showMessage(text, delayms);
}


void MyCvWindow::enablePropertiesButton()
{
    if (!vect_QActions.empty())
        vect_QActions[9]->setDisabled(false);
}


MyCvButtonbar* MyCvWindow::createButtonBar(QString name_bar)
{
    QPointer<MyCvButtonbar> t = new MyCvButtonbar(global_control_panel, name_bar);
    t->setAlignment(Qt::AlignHCenter);

    QPointer<QBoxLayout> myLayout = global_control_panel->myLayout;

    myLayout->insertLayout(myLayout->count(), t);

    return t;
}


void MyCvWindow::addSlider(MyCvWindow* w, QString name, int* value, int count, CvTrackbarCallback on_change)
{
    QPointer<MyCvTrackbar> t = new MyCvTrackbar(w, name, value, count, on_change);
    t->setAlignment(Qt::AlignHCenter);

    QPointer<QBoxLayout> myLayout;

    if (w)
    {
        myLayout = w->myBarLayout;
    }
    else
    {
        myLayout = global_control_panel->myLayout;

        //if first one, enable control panel
        if (myLayout->count() == 0)
            guiMainThread->enablePropertiesButtonEachWindow();
    }

    myLayout->insertLayout(myLayout->count(), t);
}


void MyCvWindow::addSlider2(MyCvWindow* w, QString name, int* value, int count, CvTrackbarCallback2 on_change, void* userdata)
{
    QPointer<MyCvTrackbar> t = new MyCvTrackbar(w, name, value, count, on_change, userdata);
    t->setAlignment(Qt::AlignHCenter);

    QPointer<QBoxLayout> myLayout;

    if (w)
    {
        myLayout = w->myBarLayout;
    }
    else
    {
        myLayout = global_control_panel->myLayout;

        //if first one, enable control panel
        if (myLayout->count() == 0)
            guiMainThread->enablePropertiesButtonEachWindow();
    }

    myLayout->insertLayout(myLayout->count(), t);
}


void MyCvWindow::setOpenGlDrawCallback(CvOpenGlDrawCallback callback, void* userdata)
{
    myView->setOpenGlDrawCallback(callback, userdata);
}


void MyCvWindow::makeCurrentOpenGlContext()
{
    myView->makeCurrentOpenGlContext();
}


void MyCvWindow::updateGl()
{
    myView->updateGl();
}


bool MyCvWindow::isOpenGl()
{
    return mode_display == CV_MODE_OPENGL;
}


void MyCvWindow::setViewportSize(QSize _size)
{
    resize(_size);
    myView->setSize(_size);
}


void MyCvWindow::createBarLayout()
{
    myBarLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    myBarLayout->setObjectName(QString::fromUtf8("barLayout"));
    myBarLayout->setContentsMargins(0, 0, 0, 0);
    myBarLayout->setSpacing(0);
    myBarLayout->setMargin(0);
}


void MyCvWindow::createGlobalLayout()
{
    myGlobalLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    myGlobalLayout->setObjectName(QString::fromUtf8("boxLayout"));
    myGlobalLayout->setContentsMargins(0, 0, 0, 0);
    myGlobalLayout->setSpacing(0);
    myGlobalLayout->setMargin(0);
    setMinimumSize(1, 1);

    if (param_flags == CV_WINDOW_AUTOSIZE)
    {
        /*** Rotem ***/
        //Set QLayout::SetMinAndMaxSize is shown image is too large (if image resolution is larger than screen resolution).
        ////////////////////////////////////////////////////////////////////////////
        bool is_mat_larger_then_screenresolution = isMatLargerThanScreenResolution();

        if (is_mat_larger_then_screenresolution)
        {
            myGlobalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        }
        else
        {
            myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize);
        }

        //myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize); //Original code.
        ////////////////////////////////////////////////////////////////////////////
        /*** Rotem ***/
    }
    else if (param_flags == CV_WINDOW_NORMAL)
    {
        myGlobalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    }
}


void MyCvWindow::createView()
{
#ifdef HAVE_QT_OPENGL
    if (isOpenGl())
        myView = new MyOpenGlViewPort(this);
    else
#endif
        myView = new MyDefaultViewPort(this, param_ratio_mode);
}


void MyCvWindow::createActions()
{
    vect_QActions.resize(12);

    QWidget* view = myView->getWidget();

    //if the shortcuts are changed in window_QT.h, we need to update the tooltip manually
    vect_QActions[0] = new QAction(QIcon(":/left-icon"), "Panning left (CTRL+arrowLEFT)", this);
    vect_QActions[0]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[0], SIGNAL(triggered()), view, SLOT(siftWindowOnLeft()));

    vect_QActions[1] = new QAction(QIcon(":/right-icon"), "Panning right (CTRL+arrowRIGHT)", this);
    vect_QActions[1]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[1], SIGNAL(triggered()), view, SLOT(siftWindowOnRight()));

    vect_QActions[2] = new QAction(QIcon(":/up-icon"), "Panning up (CTRL+arrowUP)", this);
    vect_QActions[2]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[2], SIGNAL(triggered()), view, SLOT(siftWindowOnUp()));

    vect_QActions[3] = new QAction(QIcon(":/down-icon"), "Panning down (CTRL+arrowDOWN)", this);
    vect_QActions[3]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[3], SIGNAL(triggered()), view, SLOT(siftWindowOnDown()) );

    vect_QActions[4] = new QAction(QIcon(":/zoom_x1-icon"), "Zoom x1 (CTRL+P)", this);
    vect_QActions[4]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[4], SIGNAL(triggered()), view, SLOT(resetZoom()));

    vect_QActions[5] = new QAction(QIcon(":/imgRegion-icon"), tr("Zoom x%1 (see label) (CTRL+X)").arg(threshold_zoom_img_region), this);
    vect_QActions[5]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[5], SIGNAL(triggered()), view, SLOT(imgRegion()));

    vect_QActions[6] = new QAction(QIcon(":/zoom_in-icon"), "Zoom in (CTRL++)", this);
    vect_QActions[6]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[6], SIGNAL(triggered()), view, SLOT(ZoomIn()));

    vect_QActions[7] = new QAction(QIcon(":/zoom_out-icon"), "Zoom out (CTRL+-)", this);
    vect_QActions[7]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[7], SIGNAL(triggered()), view, SLOT(ZoomOut()));

    vect_QActions[8] = new QAction(QIcon(":/save-icon"), "Save current image (CTRL+S)", this);
    vect_QActions[8]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[8], SIGNAL(triggered()), view, SLOT(saveView()));

    vect_QActions[9] = new QAction(QIcon(":/B0-icon"), "Change balance preset (B)", this);
    vect_QActions[9]->setIconVisibleInMenu(true);
    QObject::connect(vect_QActions[9], SIGNAL(triggered()), view, SLOT(changeBalance()));

	/*** Rotem ***/
	//Add toolbar icons for stepping backward and forward.
	//////////////////////////////////////////////////////////////////////////
	vect_QActions[10] = new QAction(QIcon(":/step_bw-icon"), "Previous (Left arrow)", this);
	vect_QActions[10]->setIconVisibleInMenu(true);
	QObject::connect(vect_QActions[10], SIGNAL(triggered()), view, SLOT(previousImage())); //Use same SLOT as left arrow key

	vect_QActions[11] = new QAction(QIcon(":/step_fw-icon"), "Next (Right arrow)", this);
	vect_QActions[11]->setIconVisibleInMenu(true);
	QObject::connect(vect_QActions[11], SIGNAL(triggered()), view, SLOT(nextImage()));  //Use same SLOT as right arrow key
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/


    //if (global_control_panel->myLayout->count() == 0)
    //    vect_QActions[9]->setDisabled(true);
}


void MyCvWindow::createShortcuts()
{
    //vect_QShortcuts.resize(10);
	vect_QShortcuts.resize(15);

    QWidget* view = myView->getWidget();

    vect_QShortcuts[0] = new QShortcut(shortcut_panning_left, this);
    QObject::connect(vect_QShortcuts[0], SIGNAL(activated()), view, SLOT(siftWindowOnLeft()));

    vect_QShortcuts[1] = new QShortcut(shortcut_panning_right, this);
    QObject::connect(vect_QShortcuts[1], SIGNAL(activated()), view, SLOT(siftWindowOnRight()));

    vect_QShortcuts[2] = new QShortcut(shortcut_panning_up, this);
    QObject::connect(vect_QShortcuts[2], SIGNAL(activated()), view, SLOT(siftWindowOnUp()));

    vect_QShortcuts[3] = new QShortcut(shortcut_panning_down, this);
    QObject::connect(vect_QShortcuts[3], SIGNAL(activated()), view, SLOT(siftWindowOnDown()));

    vect_QShortcuts[4] = new QShortcut(shortcut_zoom_normal, this);
    QObject::connect(vect_QShortcuts[4], SIGNAL(activated()), view, SLOT(resetZoom()));

    vect_QShortcuts[5] = new QShortcut(shortcut_zoom_imgRegion, this);
    QObject::connect(vect_QShortcuts[5], SIGNAL(activated()), view, SLOT(imgRegion()));

    vect_QShortcuts[6] = new QShortcut(shortcut_zoom_in, this);
    QObject::connect(vect_QShortcuts[6], SIGNAL(activated()), view, SLOT(ZoomIn()));

    vect_QShortcuts[7] = new QShortcut(shortcut_zoom_out, this);
    QObject::connect(vect_QShortcuts[7], SIGNAL(activated()), view, SLOT(ZoomOut()));

    vect_QShortcuts[8] = new QShortcut(shortcut_save_img, this);
    QObject::connect(vect_QShortcuts[8], SIGNAL(activated()), view, SLOT(saveView()));

    vect_QShortcuts[9] = new QShortcut(shortcut_properties_win, this);
    QObject::connect(vect_QShortcuts[9], SIGNAL(activated()), this, SLOT(displayPropertiesWin()));


	/*** Rotem ***/
	//"Balance" Changing.
	//////////////////////////////////////////////////////////////////////////////	
	vect_QShortcuts[10] = new QShortcut(shortcut_change_balance, this);
	QObject::connect(vect_QShortcuts[10], SIGNAL(activated()), view, SLOT(changeBalance()));
	//////////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/

	
	/*** Rotem ***/
	//Connects events to arrow keys (for some reason keyPressEvent doesn't detect arrow keys.
	//////////////////////////////////////////////////////////////////////////////
	vect_QShortcuts[11] = new QShortcut(shortcut_key_left, this);
	QObject::connect(vect_QShortcuts[11], SIGNAL(activated()), view, SLOT(leftArrowPressed()));

	vect_QShortcuts[12] = new QShortcut(shortcut_key_right, this);
	QObject::connect(vect_QShortcuts[12], SIGNAL(activated()), view, SLOT(rightArrowPressed()));

	vect_QShortcuts[13] = new QShortcut(shortcut_key_up, this);
	QObject::connect(vect_QShortcuts[13], SIGNAL(activated()), view, SLOT(upArrowPressed()));

	vect_QShortcuts[14] = new QShortcut(shortcut_key_down, this);
	QObject::connect(vect_QShortcuts[14], SIGNAL(activated()), view, SLOT(downArrowPressed()));
	//////////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


void MyCvWindow::createToolBar()
{
    myToolBar = new QToolBar(this);
    myToolBar->setFloatable(false); //is not a window
    myToolBar->setFixedHeight(28);
    myToolBar->setMinimumWidth(1);

    foreach (QAction *a, vect_QActions)
        myToolBar->addAction(a);
}


void MyCvWindow::createStatusBar()
{
    myStatusBar = new QStatusBar(this);
    myStatusBar->setSizeGripEnabled(false);
    myStatusBar->setFixedHeight(20);
    myStatusBar->setMinimumWidth(1);
    myStatusBar_msg = new QLabel;

    //I comment this because if we change the style, myview (the picture)
    //will not be the correct size anymore (will lost 2 pixel because of the borders)

    //myStatusBar_msg->setFrameStyle(QFrame::Raised);

    myStatusBar_msg->setAlignment(Qt::AlignHCenter);
    myStatusBar->addWidget(myStatusBar_msg);
}


void MyCvWindow::hideTools()
{
    if (myToolBar)
        myToolBar->hide();

    if (myStatusBar)
        myStatusBar->hide();

    if (global_control_panel)
        global_control_panel->hide();
}


void MyCvWindow::showTools()
{
    if (myToolBar)
        myToolBar->show();

    if (myStatusBar)
        myStatusBar->show();
}


MyCvWinProperties* MyCvWindow::createParameterWindow()
{
    QString name_paraWindow = QFileInfo(QApplication::applicationFilePath()).fileName() + " settings";

    MyCvWinProperties* result = new MyCvWinProperties(name_paraWindow, guiMainThread);

    return result;
}


void MyCvWindow::displayPropertiesWin()
{
    if (global_control_panel->isHidden())
        global_control_panel->show();
    else
        global_control_panel->hide();
}


//Need more test here !
void MyCvWindow::keyPressEvent(QKeyEvent *evnt)
{
    //see http://doc.trolltech.com/4.6/qt.html#Key-enum
    int key = evnt->key();

        Qt::Key qtkey = static_cast<Qt::Key>(key);
        char asciiCode = QTest::keyToAscii(qtkey);
        if (asciiCode != 0)
            key = static_cast<int>(asciiCode);
        else
            key = evnt->nativeVirtualKey(); //same codes as returned by GTK-based backend

    //control plus (Z, +, -, up, down, left, right) are used for zoom/panning functions
        if (evnt->modifiers() != Qt::ControlModifier)
        {
        mymutexKey.lock();
        last_key = key;
        mymutexKey.unlock();
        mykey_pressed.wakeAll();
        //evnt->accept();
    }

    QWidget::keyPressEvent(evnt);
}



/*** Rotem ***/
//Override Window  "change event".
////////////////////////////////////////////////////////////////////////////
//virtual
void MyCvWindow::changeEvent(QEvent *event) //CV_OVERRIDE;
{  
    bool do_repaint = false;

    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent* state_change_event = static_cast<QWindowStateChangeEvent*>(event); //Cast to QWindowStateChangeEvent

        if ((state_change_event->oldState() == Qt::WindowNoState) && (this->windowState() == Qt::WindowMaximized))
        {
            if (param_flags == CV_WINDOW_AUTOSIZE)
            {
                myGlobalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
                do_repaint = true;
            }

            //qDebug() << "Window Maximized!";
            m_is_maximized = true;
        }
        else if ((state_change_event->oldState() & Qt::WindowMaximized) && (this->windowState() == Qt::WindowNoState))
        {
            //if (param_flags == CV_WINDOW_AUTOSIZE)
            //{
            //    myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize);
            //    do_repaint = true;
            //}

            //qDebug() << "Window restored (to normal state)!";
            m_is_maximized = false;
        }
        else if ((state_change_event->oldState() & Qt::WindowMinimized) && (this->windowState() == Qt::WindowNoState))
        {
            if (param_flags == CV_WINDOW_AUTOSIZE)
            {
                //Set QLayout::SetMinAndMaxSize is shown image is too large (if image resolution is larger than screen resolution).
                bool is_mat_larger_then_screenresolution = isMatLargerThanScreenResolution();

                if (is_mat_larger_then_screenresolution)
                {
                    myGlobalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
                }
                else
                {
                    myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize);
                }

                //myGlobalLayout->setSizeConstraint(QLayout::SetFixedSize);
                do_repaint = true;
            }

            //qDebug() << "Window restored (to normal state)!";
            m_is_maximized = false;
        }
    }

    //Execute changeEvent function of parent class.
    MyCvWinModel::changeEvent(event);

    if (do_repaint)
    {
        //Repaint is not working, change visibility to false and true instead.
        this->setVisible(false);
        this->setVisible(true);
    }
}
////////////////////////////////////////////////////////////////////////////


/*** Rotem ***/
////////////////////////////////////////////////////////////////////////////
bool MyCvWindow::isMatLargerThanScreenResolution()
{
    if (myView == nullptr)
    {
        return false;
    }

    CvMat* image2Draw_mat2 = myView->getImage2Draw_mat2();

    if (image2Draw_mat2 == nullptr)
    {
        return false;
    }

    int cols = image2Draw_mat2->cols;
    int rows = image2Draw_mat2->rows;

    int screen_cols, screen_rows;
    QScreen* qscreen = getActiveScreenResolution(this, &screen_cols, &screen_rows);

    if (qscreen == nullptr)
    {
        return false;
    }

    if ((cols > screen_cols) || (rows > screen_rows))
    {
        return true;
    }

    return false;
}
////////////////////////////////////////////////////////////////////////////
/*** Rotem ***/



void MyCvWindow::icvLoadControlPanel()
{
    QSettings settings("OpenCV2", QFileInfo(QApplication::applicationFilePath()).fileName() + " control panel");

    int bsize = settings.beginReadArray("bars");

    if (bsize == global_control_panel->myLayout->layout()->count())
    {
        for (int i = 0; i < bsize; ++i)
        {
            MyCvBar* t = (MyCvBar*) global_control_panel->myLayout->layout()->itemAt(i);
            settings.setArrayIndex(i);
            if (t->type == type_CvTrackbar)
            {
                if (t->name_bar == settings.value("namebar").toString())
                {
                    ((MyCvTrackbar*)t)->slider->setValue(settings.value("valuebar").toInt());
                }
            }
            if (t->type == type_CvButtonbar)
            {
                int subsize = settings.beginReadArray(QString("buttonbar")+(char)i);

                if ( subsize == ((MyCvButtonbar*)t)->layout()->count() )
                    icvLoadButtonbar((MyCvButtonbar*)t,&settings);

                settings.endArray();
            }
        }
    }

    settings.endArray();
}


void MyCvWindow::icvSaveControlPanel()
{
    QSettings settings("OpenCV2", QFileInfo(QApplication::applicationFilePath()).fileName()+" control panel");

    settings.beginWriteArray("bars");

    for (int i = 0; i < global_control_panel->myLayout->layout()->count(); ++i)
    {
        MyCvBar* t = (MyCvBar*) global_control_panel->myLayout->layout()->itemAt(i);
        settings.setArrayIndex(i);
        if (t->type == type_CvTrackbar)
        {
            settings.setValue("namebar", QString(t->name_bar));
            settings.setValue("valuebar",((MyCvTrackbar*)t)->slider->value());
        }
        if (t->type == type_CvButtonbar)
        {
            settings.beginWriteArray(QString("buttonbar")+(char)i);
            icvSaveButtonbar((MyCvButtonbar*)t,&settings);
            settings.endArray();
        }
    }

    settings.endArray();
}


void MyCvWindow::icvSaveButtonbar(MyCvButtonbar* b, QSettings* settings)
{
    for (int i = 0, count = b->layout()->count(); i < count; ++i)
    {
        settings->setArrayIndex(i);

        QWidget* temp = (QWidget*) b->layout()->itemAt(i)->widget();
        QString myclass(QLatin1String(temp->metaObject()->className()));

        if (myclass == "MyCvPushButton")
        {
            MyCvPushButton* button = (MyCvPushButton*) temp;
            settings->setValue("namebutton", button->text());
            settings->setValue("valuebutton", int(button->isChecked()));
        }
        else if (myclass == "MyCvCheckBox")
        {
            MyCvCheckBox* button = (MyCvCheckBox*) temp;
            settings->setValue("namebutton", button->text());
            settings->setValue("valuebutton", int(button->isChecked()));
        }
        else if (myclass == "MyCvRadioButton")
        {
            MyCvRadioButton* button = (MyCvRadioButton*) temp;
            settings->setValue("namebutton", button->text());
            settings->setValue("valuebutton", int(button->isChecked()));
        }
    }
}


void MyCvWindow::icvLoadButtonbar(MyCvButtonbar* b, QSettings* settings)
{
    for (int i = 0, count = b->layout()->count(); i < count; ++i)
    {
        settings->setArrayIndex(i);

        QWidget* temp = (QWidget*) b->layout()->itemAt(i)->widget();
        QString myclass(QLatin1String(temp->metaObject()->className()));

        if (myclass == "MyCvPushButton")
        {
            MyCvPushButton* button = (MyCvPushButton*) temp;

            if (button->text() == settings->value("namebutton").toString())
                button->setChecked(settings->value("valuebutton").toInt());
        }
        else if (myclass == "MyCvCheckBox")
        {
            MyCvCheckBox* button = (MyCvCheckBox*) temp;

            if (button->text() == settings->value("namebutton").toString())
                button->setChecked(settings->value("valuebutton").toInt());
        }
        else if (myclass == "MyCvRadioButton")
        {
            MyCvRadioButton* button = (MyCvRadioButton*) temp;

            if (button->text() == settings->value("namebutton").toString())
                button->setChecked(settings->value("valuebutton").toInt());
        }

    }
}


void MyCvWindow::icvLoadTrackbars(QSettings* settings)
{
    int bsize = settings->beginReadArray("trackbars");

    //trackbar are saved in the same order, so no need to use icvFindTrackbarByName

    if (myBarLayout->layout()->count() == bsize) //if not the same number, the window saved and loaded is not the same (nb trackbar not equal)
    {
        for (int i = 0; i < bsize; ++i)
        {
            settings->setArrayIndex(i);

            MyCvTrackbar* t = (MyCvTrackbar*) myBarLayout->layout()->itemAt(i);

            if (t->name_bar == settings->value("name").toString())
                t->slider->setValue(settings->value("value").toInt());

        }
    }

    settings->endArray();
}


void MyCvWindow::icvSaveTrackbars(QSettings* settings)
{
    settings->beginWriteArray("trackbars");

    for (int i = 0; i < myBarLayout->layout()->count(); ++i)
    {
        settings->setArrayIndex(i);

        MyCvTrackbar* t = (MyCvTrackbar*) myBarLayout->layout()->itemAt(i);

        settings->setValue("name", t->name_bar);
        settings->setValue("value", t->slider->value());
    }

    settings->endArray();
}


//////////////////////////////////////////////////////
// MyOCVViewPort

MyOCVViewPort::MyOCVViewPort()
{
    mouseCallback = 0;
    mouseData = 0;
}

void MyOCVViewPort::setMouseCallBack(CvMouseCallback callback, void* param)
{
    mouseCallback = callback;
    mouseData = param;
}

void MyOCVViewPort::icvmouseEvent(QMouseEvent* evnt, type_mouse_event category)
{
    int cv_event = -1, flags = 0;

    icvmouseHandler(evnt, category, cv_event, flags);
    icvmouseProcessing(QPointF(evnt->pos()), cv_event, flags);
}

void MyOCVViewPort::icvmouseHandler(QMouseEvent* evnt, type_mouse_event category, int& cv_event, int& flags)
{
    Qt::KeyboardModifiers modifiers = evnt->modifiers();
    Qt::MouseButtons buttons = evnt->buttons();

    // This line gives excess flags flushing, with it you cannot predefine flags value.
    // icvmouseHandler called with flags == 0 where it really need.
    //flags = 0;
    if(modifiers & Qt::ShiftModifier)
        flags |= CV_EVENT_FLAG_SHIFTKEY;
    if(modifiers & Qt::ControlModifier)
        flags |= CV_EVENT_FLAG_CTRLKEY;
    if(modifiers & Qt::AltModifier)
        flags |= CV_EVENT_FLAG_ALTKEY;

    if(buttons & Qt::LeftButton)
        flags |= CV_EVENT_FLAG_LBUTTON;
    if(buttons & Qt::RightButton)
        flags |= CV_EVENT_FLAG_RBUTTON;
    if(buttons & Qt::MidButton)
        flags |= CV_EVENT_FLAG_MBUTTON;

    if (cv_event == -1) {
        if (category == mouse_wheel) {
            QWheelEvent *we = (QWheelEvent *) evnt;
            cv_event = ((we->orientation() == Qt::Vertical) ? CV_EVENT_MOUSEWHEEL : CV_EVENT_MOUSEHWHEEL);
            flags |= (we->delta() & 0xffff)<<16;
            return;
        }
        switch(evnt->button())
        {
        case Qt::LeftButton:
            cv_event = tableMouseButtons[category][0];
            flags |= CV_EVENT_FLAG_LBUTTON;
            break;
        case Qt::RightButton:
            cv_event = tableMouseButtons[category][1];
            flags |= CV_EVENT_FLAG_RBUTTON;
            break;
        case Qt::MidButton:
            cv_event = tableMouseButtons[category][2];
            flags |= CV_EVENT_FLAG_MBUTTON;
            break;
        default:
            cv_event = CV_EVENT_MOUSEMOVE;
        }
    }
}

void MyOCVViewPort::icvmouseProcessing(QPointF pt, int cv_event, int flags)
{
    if (mouseCallback)
        mouseCallback(cv_event, (int)std::round(pt.x()), (int)std::round(pt.y()), flags, mouseData);
}


//////////////////////////////////////////////////////
// MyDefaultViewPort


MyDefaultViewPort::MyDefaultViewPort(MyCvWindow* arg, int arg2) : QGraphicsView(arg), MyOCVViewPort(), image2Draw_mat(0), image2Draw_mat2(0)
{
    centralWidget = arg;
    param_keepRatio = arg2;

    setContentsMargins(0, 0, 0, 0);
    setMinimumSize(1, 1);
    setAlignment(Qt::AlignHCenter);

    setObjectName(QString::fromUtf8("graphicsView"));

    timerDisplay = new QTimer(this);
    timerDisplay->setSingleShot(true);
    connect(timerDisplay, SIGNAL(timeout()), this, SLOT(stopDisplayInfo()));

    drawInfo = false;
    mouseCoordinate  = QPoint(-1, -1);
    positionGrabbing = QPointF(0, 0);
    positionCorners  = QRect(0, 0, size().width(), size().height());


    //no border
    setStyleSheet( "QGraphicsView { border-style: none; }" );

    image2Draw_mat = cvCreateMat(viewport()->height(), viewport()->width(), CV_8UC3);
    cvZero(image2Draw_mat);

    nbChannelOriginImage = 0;


	/*** Rotem ***/
	//Holds image data before processing for display (i.e. 16 bits image).
	//////////////////////////////////////////////////////////////////////////
	image2Draw_mat2 = cvCreateMat(viewport()->height(), viewport()->width(), CV_16UC3);
	cvZero(image2Draw_mat2);
	nbChannelOriginImage2 = 0;
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/

    setInteractive(false);
    setMouseTracking(true); //receive mouse event everytime
}


MyDefaultViewPort::~MyDefaultViewPort()
{
    if (image2Draw_mat)
        cvReleaseMat(&image2Draw_mat);

	if (image2Draw_mat2)
		cvReleaseMat(&image2Draw_mat2);
}


QWidget* MyDefaultViewPort::getWidget()
{
    return this;
}


void MyDefaultViewPort::writeSettings(QSettings& settings)
{
    settings.setValue("matrix_view.m11", param_matrixWorld.m11());
    settings.setValue("matrix_view.m12", param_matrixWorld.m12());
    settings.setValue("matrix_view.m13", param_matrixWorld.m13());
    settings.setValue("matrix_view.m21", param_matrixWorld.m21());
    settings.setValue("matrix_view.m22", param_matrixWorld.m22());
    settings.setValue("matrix_view.m23", param_matrixWorld.m23());
    settings.setValue("matrix_view.m31", param_matrixWorld.m31());
    settings.setValue("matrix_view.m32", param_matrixWorld.m32());
    settings.setValue("matrix_view.m33", param_matrixWorld.m33());
}


void MyDefaultViewPort::readSettings(QSettings& settings)
{
    qreal m11 = settings.value("matrix_view.m11", param_matrixWorld.m11()).toDouble();
    qreal m12 = settings.value("matrix_view.m12", param_matrixWorld.m12()).toDouble();
    qreal m13 = settings.value("matrix_view.m13", param_matrixWorld.m13()).toDouble();
    qreal m21 = settings.value("matrix_view.m21", param_matrixWorld.m21()).toDouble();
    qreal m22 = settings.value("matrix_view.m22", param_matrixWorld.m22()).toDouble();
    qreal m23 = settings.value("matrix_view.m23", param_matrixWorld.m23()).toDouble();
    qreal m31 = settings.value("matrix_view.m31", param_matrixWorld.m31()).toDouble();
    qreal m32 = settings.value("matrix_view.m32", param_matrixWorld.m32()).toDouble();
    qreal m33 = settings.value("matrix_view.m33", param_matrixWorld.m33()).toDouble();

    param_matrixWorld = QTransform(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}


double MyDefaultViewPort::getRatio()
{
    return param_keepRatio;
}


void MyDefaultViewPort::setRatio(int flags)
{
    if (getRatio() == flags) //nothing to do
        return;

    //if valid flags
    if (flags == CV_WINDOW_FREERATIO || flags == CV_WINDOW_KEEPRATIO)
    {
        centralWidget->param_ratio_mode = flags;
        param_keepRatio = flags;
        updateGeometry();
        viewport()->update();
    }
}


void MyDefaultViewPort::updateImage(const CvArr* arr)
{
    CV_Assert(arr);

    CvMat* mat, stub;
    int origin = 0;

    if (CV_IS_IMAGE_HDR(arr))
        origin = ((IplImage*)arr)->origin;

    mat = cvGetMat(arr, &stub);

    if (!image2Draw_mat || !CV_ARE_SIZES_EQ(image2Draw_mat, mat))
    {
        if (image2Draw_mat)
            cvReleaseMat(&image2Draw_mat);

        //the image in ipl (to do a deep copy with cvCvtColor)
        image2Draw_mat = cvCreateMat(mat->rows, mat->cols, CV_8UC3);
        image2Draw_qt = QImage(image2Draw_mat->data.ptr, image2Draw_mat->cols, image2Draw_mat->rows, image2Draw_mat->step, QImage::Format_RGB888);

        //use to compute mouse coordinate, I need to update the ratio here and in resizeEvent
        ratioX = width() / float(image2Draw_mat->cols);
        ratioY = height() / float(image2Draw_mat->rows);
        updateGeometry();
    }

    nbChannelOriginImage = cvGetElemType(mat);

    cvConvertImage(mat, image2Draw_mat, (origin != 0 ? CV_CVTIMG_FLIP : 0) + CV_CVTIMG_SWAP_RB);

    viewport()->update();
}


/*** Rotem ***/
//Handle arr2, and execute original OpenCV updateImage(arr)
//arr2 holds image data before processing for display (i.e. 16 bits image).
void MyDefaultViewPort::updateImage2(const CvArr* arr, const CvArr* arr2)
{
	CV_Assert(arr);
	CV_Assert(arr2);

	CvMat* mat, stub;
	int origin = 0;

	if (CV_IS_IMAGE_HDR(arr2))
		origin = ((IplImage*)arr2)->origin;

	mat = cvGetMat(arr2, &stub);

	nbChannelOriginImage2 = cvGetElemType(mat); 

	if ((image2Draw_mat2 != NULL) || !CV_ARE_SIZES_EQ(image2Draw_mat2, mat))
	{
		if (image2Draw_mat2)
			cvReleaseMat(&image2Draw_mat2);

		//the image in ipl (to do a deep copy with cvCvtColor)
		if ((nbChannelOriginImage2 == CV_16UC3) || (nbChannelOriginImage2 == CV_16UC1))
		{
			//arr2 image is 16 bits image
			image2Draw_mat2 = cvCreateMat(mat->rows, mat->cols, CV_16UC3);
			image2Draw_qt2 = QImage(image2Draw_mat2->data.ptr, image2Draw_mat2->cols, image2Draw_mat2->rows, image2Draw_mat2->step, QImage::Format_RGB16);
		}
		else if ((nbChannelOriginImage2 == CV_8UC3) || (nbChannelOriginImage2 == CV_8UC1))
		{
			//arr2 image is 8 bits image
			image2Draw_mat2 = cvCreateMat(mat->rows, mat->cols, CV_8UC3);
			image2Draw_qt2 = QImage(image2Draw_mat2->data.ptr, image2Draw_mat2->cols, image2Draw_mat2->rows, image2Draw_mat2->step, QImage::Format_RGB888);
		}
		else
		{
			CV_Error(CV_StsError, "Unsupported type of arr2 (unsupported by Rotem)");
		}
	}

	if (nbChannelOriginImage2 == CV_16UC3)
	{
		if (origin == 0)
		{
			cvCvtColor(mat, image2Draw_mat2, CV_BGR2RGB);
		}
		else
		{
			//???
			CV_Error(CV_StsError, "origin != 0 in updateImage2 (unsupported by Rotem)");
			//cvCopy(mat, image2Draw_mat2);
			cvCvtColor(mat, image2Draw_mat2, CV_BGR2RGB);
		}
	}
	else if (nbChannelOriginImage2 == CV_16UC1)
	{
		if (origin == 0)
		{
			cvCvtColor(mat, image2Draw_mat2, CV_GRAY2BGR);
		}
		else
		{
			//???
			CV_Error(CV_StsError, "origin != 0 in updateImage2 (unsupported by Rotem)");
			cvCvtColor(mat, image2Draw_mat2, CV_GRAY2BGR);
		}
	}
	else
	{
		cvConvertImage(mat, image2Draw_mat2, (origin != 0 ? CV_CVTIMG_FLIP : 0) + CV_CVTIMG_SWAP_RB);
	}

	//Execute original OpenCV updateImage function
	updateImage(arr);
}


void MyDefaultViewPort::startDisplayInfo(QString text, int delayms)
{
    if (timerDisplay->isActive())
        stopDisplayInfo();

    infoText = text;
    if (delayms > 0) timerDisplay->start(delayms);
    drawInfo = true;
}


void MyDefaultViewPort::setOpenGlDrawCallback(CvOpenGlDrawCallback /*callback*/, void* /*userdata*/)
{
    CV_Error(CV_OpenGlNotSupported, "Window doesn't support OpenGL");
}


void MyDefaultViewPort::makeCurrentOpenGlContext()
{
    CV_Error(CV_OpenGlNotSupported, "Window doesn't support OpenGL");
}


void MyDefaultViewPort::updateGl()
{
    CV_Error(CV_OpenGlNotSupported, "Window doesn't support OpenGL");
}


//Note: move 2 percent of the window
void MyDefaultViewPort::siftWindowOnLeft()
{
    double delta = 2.0 * (double)width() / (100.0 * param_matrixWorld.m11());
    moveView(QPointF(delta, 0));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


//Note: move 2 percent of the window
void MyDefaultViewPort::siftWindowOnRight()
{
    double delta = -2.0 * (double)width() / (100.0 * param_matrixWorld.m11());
    moveView(QPointF(delta, 0));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


//Note: move 2 percent of the window
void MyDefaultViewPort::siftWindowOnUp()
{
    double delta = 2.0 * (double)height() / (100.0 * param_matrixWorld.m11());
    moveView(QPointF(0, delta));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


//Note: move 2 percent of the window
void MyDefaultViewPort::siftWindowOnDown()
{
    double delta = -2.0 * (double)height() / (100.0 * param_matrixWorld.m11());
    moveView(QPointF(0, delta));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


void MyDefaultViewPort::imgRegion()
{
    scaleView((threshold_zoom_img_region / param_matrixWorld.m11() - 1) * 5, QPointF(size().width() / 2, size().height() / 2));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


void MyDefaultViewPort::resetZoom()
{
    param_matrixWorld.reset();
    controlImagePosition();

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
    setCursor(Qt::ArrowCursor);
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


void MyDefaultViewPort::ZoomIn()
{
    scaleView(0.5, QPointF(size().width() / 2, size().height() / 2));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}


void MyDefaultViewPort::ZoomOut()
{
    scaleView(-0.5, QPointF(size().width() / 2, size().height() / 2));

	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
	this->repaint(); //Force repaint after action
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/
}



//can save as JPG, JPEG, BMP, PNG
void MyDefaultViewPort::saveView()
{
    QDate date_d = QDate::currentDate();
    QString date_s = date_d.toString("dd.MM.yyyy");
    QString name_s = centralWidget->windowTitle() + "_screenshot_" + date_s;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File %1").arg(name_s), name_s + ".png", tr("Images (*.png *.jpg *.bmp *.jpeg)"));

    if (!fileName.isEmpty()) //save the picture
    {
        QString extension = fileName.right(3);

        // Create a new pixmap to render the viewport into
        QPixmap viewportPixmap(viewport()->size());
        viewport()->render(&viewportPixmap);

        // Save it..
        if (QString::compare(extension, "png", Qt::CaseInsensitive) == 0)
        {
            viewportPixmap.save(fileName, "PNG");
            return;
        }

        if (QString::compare(extension, "jpg", Qt::CaseInsensitive) == 0)
        {
            viewportPixmap.save(fileName, "JPG");
            return;
        }

        if (QString::compare(extension, "bmp", Qt::CaseInsensitive) == 0)
        {
            viewportPixmap.save(fileName, "BMP");
            return;
        }

        if (QString::compare(extension, "jpeg", Qt::CaseInsensitive) == 0)
        {
            viewportPixmap.save(fileName, "JPEG");
            return;
        }

        CV_Error(CV_StsNullPtr, "file extension not recognized, please choose between JPG, JPEG, BMP or PNG");
    }
}



/*** Rotem ***/
//Events for arrow keys (for some reason keyPressEvent doesn't detect arrow keys).
//////////////////////////////////////////////////////////////////////////////
void MyDefaultViewPort::leftArrowPressed()
{
	last_key = 1000001;
}

void MyDefaultViewPort::rightArrowPressed()
{
	last_key = 1000002;
}

void MyDefaultViewPort::upArrowPressed()
{
	last_key = 1000003;
}

void MyDefaultViewPort::downArrowPressed()
{
	last_key = 1000004;
}


//Toolbar previous icon pressed
void MyDefaultViewPort::previousImage()
{
	leftArrowPressed();
}

//Toolbar next icon pressed
void MyDefaultViewPort::nextImage()
{
	rightArrowPressed();
}


//Toolbar next icon pressed
void MyDefaultViewPort::changeBalance()
{
	last_key = 'B';
}
//////////////////////////////////////////////////////////////////////////////
/*** Rotem ***/




void MyDefaultViewPort::contextMenuEvent(QContextMenuEvent* evnt)
{
    if (centralWidget->vect_QActions.size() > 0)
    {
        QMenu menu(this);

        foreach (QAction *a, centralWidget->vect_QActions)
            menu.addAction(a);

        menu.exec(evnt->globalPos());
    }
}


void MyDefaultViewPort::resizeEvent(QResizeEvent* evnt)
{
    controlImagePosition();

    //use to compute mouse coordinate, I need to update the ratio here and in resizeEvent
    ratioX = width() / float(image2Draw_mat->cols);
    ratioY = height() / float(image2Draw_mat->rows);

    if (param_keepRatio == CV_WINDOW_KEEPRATIO)//to keep the same aspect ratio
    {
        QSize newSize = QSize(image2Draw_mat->cols, image2Draw_mat->rows);
        newSize.scale(evnt->size(), Qt::KeepAspectRatio);

        //imageWidth/imageHeight = newWidth/newHeight +/- epsilon
        //ratioX = ratioY +/- epsilon
        //||ratioX - ratioY|| = epsilon
        if (fabs(ratioX - ratioY) * 100 > ratioX) //avoid infinity loop / epsilon = 1% of ratioX
        {
            resize(newSize);
            viewport()->resize(newSize);

            //move to the middle
            //newSize get the delta offset to place the picture in the middle of its parent
            newSize = (evnt->size() - newSize) / 2;

            //if the toolbar is displayed, avoid drawing myview on top of it
            if (centralWidget->myToolBar)
                if(!centralWidget->myToolBar->isHidden())
                    newSize += QSize(0, centralWidget->myToolBar->height());

            move(newSize.width(), newSize.height());
        }
    }

    return QGraphicsView::resizeEvent(evnt);
}


void MyDefaultViewPort::wheelEvent(QWheelEvent* evnt)
{
    icvmouseEvent((QMouseEvent *)evnt, mouse_wheel);

    scaleView(evnt->delta() / 240.0, evnt->pos());
    viewport()->update();

    QWidget::wheelEvent(evnt);
}


void MyDefaultViewPort::mousePressEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_down);

    if (param_matrixWorld.m11()>1)
    {
        setCursor(Qt::ClosedHandCursor);
        positionGrabbing = evnt->pos();
    }

    QWidget::mousePressEvent(evnt);
}


void MyDefaultViewPort::mouseReleaseEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_up);

    if (param_matrixWorld.m11()>1)
        setCursor(Qt::OpenHandCursor);

    QWidget::mouseReleaseEvent(evnt);
}


void MyDefaultViewPort::mouseDoubleClickEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_dbclick);
    QWidget::mouseDoubleClickEvent(evnt);
}


void MyDefaultViewPort::mouseMoveEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_move);

    if (param_matrixWorld.m11() > 1 && evnt->buttons() == Qt::LeftButton)
    {
        QPoint pt = evnt->pos();
        QPointF dxy = (pt - positionGrabbing)/param_matrixWorld.m11();
        positionGrabbing = pt;
        moveView(dxy);
    }

    //I update the statusbar here because if the user does a cvWaitkey(0) (like with inpaint.cpp)
    //the status bar will only be repaint when a click occurs.
    if (centralWidget->myStatusBar)
        viewport()->update();

    QWidget::mouseMoveEvent(evnt);
}


void MyDefaultViewPort::paintEvent(QPaintEvent* evnt)
{
    QPainter myPainter(viewport());
    myPainter.setWorldTransform(param_matrixWorld);

    draw2D(&myPainter);

    //Now disable matrixWorld for overlay display
    myPainter.setWorldMatrixEnabled(false);

    //overlay pixel values if zoomed in far enough
    if (param_matrixWorld.m11()*ratioX >= threshold_zoom_img_region &&
        param_matrixWorld.m11()*ratioY >= threshold_zoom_img_region)
    {
        drawImgRegion(&myPainter);
    }

    //in mode zoom/panning
    if (param_matrixWorld.m11() > 1)
    {
        drawViewOverview(&myPainter);
    }

    //for information overlay
    if (drawInfo)
        drawInstructions(&myPainter);

    //for statusbar
    if (centralWidget->myStatusBar)
        drawStatusBar();

    QGraphicsView::paintEvent(evnt);
}


void MyDefaultViewPort::stopDisplayInfo()
{
    timerDisplay->stop();
    drawInfo = false;
}


inline bool MyDefaultViewPort::isSameSize(IplImage* img1, IplImage* img2)
{
    return img1->width == img2->width && img1->height == img2->height;
}


void MyDefaultViewPort::controlImagePosition()
{
    qreal left, top, right, bottom;

    //after check top-left, bottom right corner to avoid getting "out" during zoom/panning
    param_matrixWorld.map(0,0,&left,&top);

    if (left > 0)
    {
        param_matrixWorld.translate(-left,0);
        left = 0;
    }
    if (top > 0)
    {
        param_matrixWorld.translate(0,-top);
        top = 0;
    }
    //-------

    QSize sizeImage = size();
    param_matrixWorld.map(sizeImage.width(),sizeImage.height(),&right,&bottom);
    if (right < sizeImage.width())
    {
        param_matrixWorld.translate(sizeImage.width()-right,0);
        right = sizeImage.width();
    }
    if (bottom < sizeImage.height())
    {
        param_matrixWorld.translate(0,sizeImage.height()-bottom);
        bottom = sizeImage.height();
    }

    //save corner position
    positionCorners.setTopLeft(QPoint((int)std::round(left), (int)std::round(top)));
    positionCorners.setBottomRight(QPoint((int)std::round(right), (int)std::round(bottom)));
    //save also the inv matrix
    matrixWorld_inv = param_matrixWorld.inverted();

    //viewport()->update();
}

void MyDefaultViewPort::moveView(QPointF delta)
{
    param_matrixWorld.translate(delta.x(),delta.y());
    controlImagePosition();
    viewport()->update();
}

//factor is -0.5 (zoom out) or 0.5 (zoom in)
void MyDefaultViewPort::scaleView(qreal factor,QPointF center)
{
    factor/=5;//-0.1 <-> 0.1
    factor+=1;//0.9 <-> 1.1

    //limit zoom out ---
    if (param_matrixWorld.m11()==1 && factor < 1)
        return;

    if (param_matrixWorld.m11()*factor<1)
        factor = 1/param_matrixWorld.m11();


    //limit zoom int ---
    if (param_matrixWorld.m11()>100 && factor > 1)
        return;

    //inverse the transform
    int a, b;
    matrixWorld_inv.map((int)std::round(center.x()), (int)std::round(center.y()), &a, &b);

    param_matrixWorld.translate(a-factor*a,b-factor*b);
    param_matrixWorld.scale(factor,factor);

    controlImagePosition();

    //display new zoom
    if (centralWidget->myStatusBar)
        centralWidget->displayStatusBar(tr("Zoom: %1%").arg(param_matrixWorld.m11()*100),1000);

    if (param_matrixWorld.m11()>1)
        setCursor(Qt::OpenHandCursor);
    else
        unsetCursor();
}




void MyDefaultViewPort::icvmouseProcessing(QPointF pt, int cv_event, int flags)
{
    //to convert mouse coordinate
    qreal pfx, pfy;
    matrixWorld_inv.map(pt.x(),pt.y(),&pfx,&pfy);

    mouseCoordinate.rx()=(int)floor(pfx/ratioX);
    mouseCoordinate.ry()=(int)floor(pfy/ratioY);

    MyOCVViewPort::icvmouseProcessing(QPointF(mouseCoordinate), cv_event, flags);
}


QSize MyDefaultViewPort::sizeHint() const
{
    if(image2Draw_mat)
        return QSize(image2Draw_mat->cols, image2Draw_mat->rows);
    else
        return QGraphicsView::sizeHint();
}


void MyDefaultViewPort::draw2D(QPainter *painter)
{
    image2Draw_qt = QImage(image2Draw_mat->data.ptr, image2Draw_mat->cols, image2Draw_mat->rows,image2Draw_mat->step,QImage::Format_RGB888);
    painter->drawImage(QRect(0,0,viewport()->width(),viewport()->height()), image2Draw_qt, QRect(0,0, image2Draw_qt.width(), image2Draw_qt.height()) );
}

//only if CV_8UC1 or CV_8UC3
void MyDefaultViewPort::drawStatusBar()
{
    if (nbChannelOriginImage!=CV_8UC1 && nbChannelOriginImage!=CV_8UC3)
        return;

	if (mouseCoordinate.x() >= 0 &&
		mouseCoordinate.y() >= 0 &&
		mouseCoordinate.x() < image2Draw_qt.width() &&
		mouseCoordinate.y() < image2Draw_qt.height())
		//  if (mouseCoordinate.x()>=0 && mouseCoordinate.y()>=0)
	{
		/*** Rotem ***/
		//Get the pixel value from image2Draw_qt2 instead of image2Draw_qt
		//image2Draw_qt2 is assumed to be the original image before processing for display (i.e. 16 bits image).
		//////////////////////////////////////////////////////////////////////////
		if (image2Draw_qt2.isNull() || (nbChannelOriginImage2 == CV_8UC1) || (nbChannelOriginImage2 == CV_8UC3))
		{
			//Original code - handle 
			QRgb rgbValue = image2Draw_qt.pixel(mouseCoordinate);

			if (nbChannelOriginImage == CV_8UC3)
			{
				centralWidget->myStatusBar_msg->setText(tr("<font color='black'>(x=%1, y=%2): </font>")
					.arg(mouseCoordinate.x())
					.arg(mouseCoordinate.y()) +
					tr("<font color='red'>R:%3 </font>").arg(qRed(rgbValue)) +//.arg(value.val[0])+
					tr("<font color='green'>G:%4 </font>").arg(qGreen(rgbValue)) +//.arg(value.val[1])+
					tr("<font color='blue'>B:%5</font>").arg(qBlue(rgbValue))//.arg(value.val[2])
				);
			}

			if (nbChannelOriginImage == CV_8UC1)
			{
				//all the channel have the same value (because of cvconvertimage), so only the r channel is dsplayed
				centralWidget->myStatusBar_msg->setText(tr("<font color='black'>(x=%1, y=%2): </font>")
					.arg(mouseCoordinate.x())
					.arg(mouseCoordinate.y()) +
					tr("<font color='grey'>L:%3 </font>").arg(qRed(rgbValue))
				);
			}
		}
		else
		{
			const int x = mouseCoordinate.x();
			const int y = mouseCoordinate.y();
			const int step = image2Draw_mat2->step;
			const unsigned short *P = (unsigned short*)(image2Draw_mat2->data.ptr + step*y);	//Points the pixel in coordinate (x,y) in image image2Draw_mat2
			
			if (nbChannelOriginImage2 == CV_16UC3)
			{
				int r_value = P[x*3 + 0];
				int g_value = P[x*3 + 1];
				int b_value = P[x*3 + 2];

				centralWidget->myStatusBar_msg->setText(tr("<font color='black'>(x=%1, y=%2): </font>")
					.arg(x)
					.arg(y) +
					tr("<font color='red'>R:%3 </font>").arg(r_value) +
					tr("<font color='green'>G:%4 </font>").arg(g_value) +
					tr("<font color='blue'>B:%5</font>").arg(b_value));
			}

			if (nbChannelOriginImage2 == CV_16UC1)
			{
				//QRgb rgbValue = image2Draw_qt.pixel(mouseCoordinate);

				int gray_value = P[x*3];

				//all the channel have the same value (because of cvconvertimage), so only the r channel is displayed
				centralWidget->myStatusBar_msg->setText(tr("<font color='black'>(x=%1, y=%2): </font>")
					.arg(x)
					.arg(y) +
					//tr("<font color='black'>L8:%3 </font>").arg(qRed(rgbValue)) + /*** Temporary **/
					tr("<font color='grey'>L:%3 </font>").arg(gray_value));
			}
		}
		//////////////////////////////////////////////////////////////////////////
		/*** Rotem ***/
    }
}

//accept only CV_8UC1 and CV_8UC8 image for now
void MyDefaultViewPort::drawImgRegion(QPainter *painter)
{
    if (nbChannelOriginImage!=CV_8UC1 && nbChannelOriginImage!=CV_8UC3)
        return;

    double pixel_width = param_matrixWorld.m11()*ratioX;
    double pixel_height = param_matrixWorld.m11()*ratioY;

    qreal offsetX = param_matrixWorld.dx()/pixel_width;
    offsetX = offsetX - floor(offsetX);
    qreal offsetY = param_matrixWorld.dy()/pixel_height;
    offsetY = offsetY - floor(offsetY);

    QSize view = size();
    QVarLengthArray<QLineF, 30> linesX;
    for (qreal _x = offsetX*pixel_width; _x < view.width(); _x += pixel_width )
        linesX.append(QLineF(_x, 0, _x, view.height()));

    QVarLengthArray<QLineF, 30> linesY;
    for (qreal _y = offsetY*pixel_height; _y < view.height(); _y += pixel_height )
        linesY.append(QLineF(0, _y, view.width(), _y));


    QFont f = painter->font();
    int original_font_size = f.pointSize();
    //change font size
    //f.setPointSize(4+(param_matrixWorld.m11()-threshold_zoom_img_region)/5);
    f.setPixelSize((int)(10+(pixel_height-threshold_zoom_img_region)/5));
    painter->setFont(f);


    for (int j=-1;j<height()/pixel_height;j++)//-1 because display the pixels top rows left columns
    {
        for (int i=-1;i<width()/pixel_width;i++)//-1
        {
            // Calculate top left of the pixel's position in the viewport (screen space)
            QPointF pos_in_view((i+offsetX)*pixel_width, (j+offsetY)*pixel_height);

            // Calculate top left of the pixel's position in the image (image space)
            QPointF pos_in_image = matrixWorld_inv.map(pos_in_view);// Top left of pixel in view
            pos_in_image.rx() = pos_in_image.x()/ratioX;
            pos_in_image.ry() = pos_in_image.y()/ratioY;
            QPoint point_in_image((int)(pos_in_image.x() + 0.5f), (int)(pos_in_image.y() + 0.5f));// Add 0.5 for rounding

            QRgb rgbValue;
            if (image2Draw_qt.valid(point_in_image))
                rgbValue = image2Draw_qt.pixel(point_in_image);
            else
                rgbValue = qRgb(0,0,0);

			/*** Rotem ***/
			//Get the pixel value from image2Draw_qt2 instead of image2Draw_qt
			//image2Draw_qt2 is assumed to be the original image before processing for display (i.e. 16 bits image).
			//////////////////////////////////////////////////////////////////////////
			if (image2Draw_qt2.isNull() || (nbChannelOriginImage2 == CV_8UC1) || (nbChannelOriginImage2 == CV_8UC3))
			{
				if (nbChannelOriginImage == CV_8UC3)
				{
					//for debug
					/*
					val = tr("%1 %2").arg(point2.x()).arg(point2.y());
					painter->setPen(QPen(Qt::black, 1));
					painter->drawText(QRect(point1.x(),point1.y(),param_matrixWorld.m11(),param_matrixWorld.m11()/2),
						Qt::AlignCenter, val);
					*/
					QString val;

					val = tr("%1").arg(qRed(rgbValue));
					painter->setPen(QPen(Qt::red, 1));
					painter->drawText(QRect((int)pos_in_view.x(), (int)pos_in_view.y(), (int)pixel_width, (int)(pixel_height / 3)),
						Qt::AlignCenter, val);

					val = tr("%1").arg(qGreen(rgbValue));
					painter->setPen(QPen(Qt::green, 1));
					painter->drawText(QRect((int)pos_in_view.x(), (int)(pos_in_view.y() + pixel_height / 3), (int)pixel_width, (int)(pixel_height / 3)),
						Qt::AlignCenter, val);

					val = tr("%1").arg(qBlue(rgbValue));
					painter->setPen(QPen(Qt::blue, 1));
					painter->drawText(QRect((int)pos_in_view.x(), (int)(pos_in_view.y() + 2 * pixel_height / 3), (int)pixel_width, (int)(pixel_height / 3)),
						Qt::AlignCenter, val);

				}

				if (nbChannelOriginImage == CV_8UC1)
				{
					QString val = tr("%1").arg(qRed(rgbValue));
					int pixel_brightness_value = qRed(rgbValue);
					int text_brightness_value = 0;

					text_brightness_value = pixel_brightness_value > 127 ? pixel_brightness_value - 127 : 127 + pixel_brightness_value;
					painter->setPen(QPen(QColor(text_brightness_value, text_brightness_value, text_brightness_value)));
					painter->drawText(QRect((int)pos_in_view.x(), (int)pos_in_view.y(), (int)pixel_width, (int)pixel_height),
						Qt::AlignCenter, val);
				}
			}
			else
			{
				int r_value = 0;
				int g_value = 0;
				int b_value = 0;

				if (image2Draw_qt.valid(point_in_image))
				{
					rgbValue = image2Draw_qt.pixel(point_in_image);

					const int x = (int)floor(pos_in_image.x() + 0.5);	//Add 0.5 for rounding
					const int y = (int)floor(pos_in_image.y() + 0.5);	//Add 0.5 for rounding
					const int step = image2Draw_mat2->step;
					const unsigned short *P = (unsigned short*)(image2Draw_mat2->data.ptr + step * y);	//Points the pixel in coordinate (x,y) in image image2Draw_mat2

					r_value = P[x * 3 + 0];
					g_value = P[x * 3 + 1];
					b_value = P[x * 3 + 2];
				}
				else
				{
					rgbValue = qRgb(0, 0, 0);
				}


				if (nbChannelOriginImage2 == CV_16UC3)
				{
					QString val;

					val = tr("%1").arg(r_value);
					painter->setPen(QPen(Qt::red, 1));
					painter->drawText(QRect((int)pos_in_view.x(), (int)pos_in_view.y(), (int)pixel_width, (int)(pixel_height / 3)), Qt::AlignCenter, val);

					val = tr("%1").arg(g_value);
					painter->setPen(QPen(Qt::green, 1));
					painter->drawText(QRect((int)pos_in_view.x(), (int)(pos_in_view.y() + pixel_height / 3), (int)pixel_width, (int)(pixel_height / 3)), Qt::AlignCenter, val);

					val = tr("%1").arg(b_value);
					painter->setPen(QPen(Qt::blue, 1));
					painter->drawText(QRect((int)pos_in_view.x(), (int)(pos_in_view.y() + 2 * pixel_height / 3), (int)pixel_width, (int)(pixel_height / 3)), Qt::AlignCenter, val);

				}

				if (nbChannelOriginImage2 == CV_16UC1)
				{
					QString val = tr("%1").arg(r_value);
					int pixel_brightness_value = qRed(rgbValue);
					int text_brightness_value = 0;

					text_brightness_value = pixel_brightness_value > 127 ? pixel_brightness_value - 127 : 127 + pixel_brightness_value;
					painter->setPen(QPen(QColor(text_brightness_value, text_brightness_value, text_brightness_value)));
					painter->drawText(QRect((int)pos_in_view.x(), (int)pos_in_view.y(), (int)pixel_width, (int)pixel_height), Qt::AlignCenter, val);

					/*** Temporary ***/
					//val = tr("%1").arg(qRed(rgbValue));
					//painter->setPen(QPen(Qt::blue, 1));
					//painter->drawText(QRect(pos_in_view.x(), pos_in_view.y() + 2 * pixel_height / 3, pixel_width, pixel_height / 3),
					//	Qt::AlignCenter, val);
					/*** Temporary ***/
				}
			}
        }
    }

    painter->setPen(QPen(Qt::black, 1));
    painter->drawLines(linesX.data(), linesX.size());
    painter->drawLines(linesY.data(), linesY.size());

    //restore font size
    f.setPointSize(original_font_size);
    painter->setFont(f);
}

void MyDefaultViewPort::drawViewOverview(QPainter *painter)
{
    QSize viewSize = size();
    viewSize.scale ( 100, 100,Qt::KeepAspectRatio );

    const int margin = 5;

    //draw the image's location
    painter->setBrush(QColor(0, 0, 0, 127));
    painter->setPen(Qt::darkGreen);
    painter->drawRect(QRect(width()-viewSize.width()-margin, 0,viewSize.width(),viewSize.height()));

    //daw the view's location inside the image
    qreal ratioSize = 1/param_matrixWorld.m11();
    qreal ratioWindow = (qreal)(viewSize.height())/(qreal)(size().height());
    painter->setPen(Qt::darkBlue);
    painter->drawRect(QRectF(width()-viewSize.width()-positionCorners.left()*ratioSize*ratioWindow-margin,
        -positionCorners.top()*ratioSize*ratioWindow,
        (viewSize.width()-1)*ratioSize,
        (viewSize.height()-1)*ratioSize)
        );
}

void MyDefaultViewPort::drawInstructions(QPainter *painter)
{
    QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());

    QRect qrect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
        Qt::AlignCenter | Qt::TextWordWrap, infoText);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->fillRect(QRect(0, 0, width(), qrect.height() + 2*border),
        QColor(0, 0, 0, 127));
    painter->setPen(Qt::white);
    painter->fillRect(QRect(0, 0, width(), qrect.height() + 2*border),
        QColor(0, 0, 0, 127));

    painter->drawText((width() - qrect.width())/2, border,
        qrect.width(), qrect.height(),
        Qt::AlignCenter | Qt::TextWordWrap, infoText);
}


void MyDefaultViewPort::setSize(QSize /*size_*/)
{
}


//////////////////////////////////////////////////////
// MyOpenGlViewPort

#ifdef HAVE_QT_OPENGL

MyOpenGlViewPort::MyOpenGlViewPort(QWidget* _parent) : QGLWidget(_parent), MyOCVViewPort(), size(-1, -1)
{
    glDrawCallback = 0;
    glDrawData = 0;
}

MyOpenGlViewPort::~MyOpenGlViewPort()
{
}

QWidget* MyOpenGlViewPort::getWidget()
{
    return this;
}


void MyOpenGlViewPort::writeSettings(QSettings& /*settings*/)
{
}

void MyOpenGlViewPort::readSettings(QSettings& /*settings*/)
{
}

double MyOpenGlViewPort::getRatio()
{
    return (double)width() / height();
}

void MyOpenGlViewPort::setRatio(int /*flags*/)
{
}

void MyOpenGlViewPort::updateImage(const CvArr* /*arr*/)
{
}

void MyOpenGlViewPort::updateImage2(const CvArr* /*arr*/, const CvArr* /*arr2*/) /*** Rotem ***/
{
}


void MyOpenGlViewPort::startDisplayInfo(QString /*text*/, int /*delayms*/)
{
}

void MyOpenGlViewPort::setOpenGlDrawCallback(CvOpenGlDrawCallback callback, void* userdata)
{
    glDrawCallback = callback;
    glDrawData = userdata;
}

void MyOpenGlViewPort::makeCurrentOpenGlContext()
{
    makeCurrent();
}

void MyOpenGlViewPort::updateGl()
{
    QGLWidget::updateGL();
}


/*** Rotem ***/
//c:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include\gl\GL.h
#ifndef GL_PERSPECTIVE_CORRECTION_HINT
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#endif

#ifndef GL_NICEST
#define GL_NICEST                         0x1102
#endif
/*** Rotem ***/

void MyOpenGlViewPort::initializeGL()
{
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void MyOpenGlViewPort::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void MyOpenGlViewPort::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glDrawCallback)
        glDrawCallback(glDrawData);
}


void MyOpenGlViewPort::wheelEvent(QWheelEvent* evnt)
{
    icvmouseEvent((QMouseEvent *)evnt, mouse_wheel);
    QGLWidget::wheelEvent(evnt);
}

void MyOpenGlViewPort::mousePressEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_down);
    QGLWidget::mousePressEvent(evnt);
}

void MyOpenGlViewPort::mouseReleaseEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_up);
    QGLWidget::mouseReleaseEvent(evnt);
}

void MyOpenGlViewPort::mouseDoubleClickEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_dbclick);
    QGLWidget::mouseDoubleClickEvent(evnt);
}

void MyOpenGlViewPort::mouseMoveEvent(QMouseEvent* evnt)
{
    icvmouseEvent(evnt, mouse_move);
    QGLWidget::mouseMoveEvent(evnt);
}


QSize MyOpenGlViewPort::sizeHint() const
{
    if (size.width() > 0 && size.height() > 0)
        return size;

    return QGLWidget::sizeHint();
}

void MyOpenGlViewPort::setSize(QSize size_)
{
    size = size_;
    updateGeometry();
}

#endif

#endif // HAVE_QT