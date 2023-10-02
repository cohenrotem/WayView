// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

// TODO: add headers that you want to pre-compile here

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

#if defined _WIN32 || defined WINCE
#include <windows.h>
#include <shellapi.h>
#undef small
#undef min
#undef max
#undef abs
#endif


#define HAVE_QT

//???
//#define HAVE_QT_OPENGL

/*** Rotem ***/
//Avoid usage of OpenGL even if we have OpenGL.
//////////////////////////////////////////////////////////////////////////
#undef HAVE_QT_OPENGL
//////////////////////////////////////////////////////////////////////////
/*** Rotem ***/



//#ifdef HAVE_TEGRA_OPTIMIZATION
//#include "opencv2/highgui/highgui_tegra.hpp"
//#endif

/* Errors */
#define HG_OK          0 /* Don't bet on it! */
#define HG_BADNAME    -1 /* Bad window or file name */
#define HG_INITFAILED -2 /* Can't initialize HigHGUI */
#define HG_WCFAILED   -3 /* Can't create a window */
#define HG_NULLPTR    -4 /* The null pointer where it should not appear */
#define HG_BADPARAM   -5

#define __BEGIN__ __CV_BEGIN__
#define __END__  __CV_END__
#define EXIT __CV_EXIT__

#define CV_WINDOW_MAGIC_VAL     0x00420042
#define CV_TRACKBAR_MAGIC_VAL   0x00420043

//Yannick Verdie 2010, Max Kostin 2015
//void cvSetModeWindow_W32(const char* name, double prop_value);
//void cvSetModeWindow_GTK(const char* name, double prop_value);
//void cvSetModeWindow_CARBON(const char* name, double prop_value);
//void cvSetModeWindow_COCOA(const char* name, double prop_value);
//void cvSetModeWindow_WinRT(const char* name, double prop_value);
//
//CvRect cvGetWindowRect_W32(const char* name);
//CvRect cvGetWindowRect_GTK(const char* name);
//CvRect cvGetWindowRect_CARBON(const char* name);
//CvRect cvGetWindowRect_COCOA(const char* name);
//
//double cvGetModeWindow_W32(const char* name);
//double cvGetModeWindow_GTK(const char* name);
//double cvGetModeWindow_CARBON(const char* name);
//double cvGetModeWindow_COCOA(const char* name);
//double cvGetModeWindow_WinRT(const char* name);
//
//double cvGetPropWindowAutoSize_W32(const char* name);
//double cvGetPropWindowAutoSize_GTK(const char* name);
//
//double cvGetRatioWindow_W32(const char* name);
//double cvGetRatioWindow_GTK(const char* name);
//
//double cvGetOpenGlProp_W32(const char* name);
//double cvGetOpenGlProp_GTK(const char* name);
//
//double cvGetPropVisible_W32(const char* name);

//for QT
#if defined (HAVE_QT)
CvRect mycvGetWindowRect_QT(const char* name);
double mycvGetModeWindow_QT(const char* name);
void mycvSetModeWindow_QT(const char* name, double prop_value);

double mycvGetPropWindow_QT(const char* name);
void mycvSetPropWindow_QT(const char* name, double prop_value);

double mycvGetRatioWindow_QT(const char* name);
void mycvSetRatioWindow_QT(const char* name, double prop_value);

double mycvGetOpenGlProp_QT(const char* name);
double mycvGetPropVisible_QT(const char* name);
#endif




#endif //PCH_H
