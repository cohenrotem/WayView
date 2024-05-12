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
#ifndef __MY_OPENCV_HIGHGUI_QT_H__
#define __MY_OPENCV_HIGHGUI_QT_H__

#pragma warning(push)
#pragma warning(disable: 4127)  //qvarlengtharray.h(169,1): warning C4127: conditional expression is constant

//#include "precomp.hpp"


//#include "pch.h"

//The following part was copied from "pch.h"
//////////////////////////////////////////////////////////////////////////
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
#undef min
#undef max
#undef abs
#endif

#define HAVE_QT

/*** Rotem ***/
//Avoid usage of OpenGL even if we have OpenGL.
//#define HAVE_QT_OPENGL
#undef HAVE_QT_OPENGL
/*** Rotem ***/

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
//////////////////////////////////////////////////////////////////////////



#ifndef _DEBUG
#define QT_NO_DEBUG_OUTPUT
#endif

#if defined( HAVE_QT_OPENGL )
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>
#else
#include <QtWidgets/QtWidgets> /*** Rotem ***/
#endif

#include <QtCore/QAbstractEventDispatcher>
#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QBoxLayout>
#include <QtCore/QSettings>
#include <QtCore/qtimer.h>
#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QWaitCondition>
#include <QtGui/QKeyEvent>
#include <QtCore/QMetaObject>
#include <QtCore/QPointer>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtCore/QIODevice>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QStatusBar>
#include <QtCore/QVarLengthArray>
#include <QtCore/QFileInfo>
#include <QtCore/QDate>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QToolBar>

#include <QtWidgets/QAction>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QMenu>
#include <QtTest/QTest>

#pragma warning(pop)


#define MY_CV_IMPL extern "C"


//start private enum
enum { CV_MODE_NORMAL = 0, CV_MODE_OPENGL = 1 };

//we can change the keyboard shortcuts from here !
enum {	shortcut_zoom_normal 	= Qt::CTRL + Qt::Key_Z,
        shortcut_zoom_imgRegion = Qt::CTRL + Qt::Key_X,
        shortcut_save_img		= Qt::CTRL + Qt::Key_S,
        shortcut_properties_win	= Qt::CTRL + Qt::Key_P,
        shortcut_zoom_in 		= Qt::CTRL + Qt::Key_Plus,//QKeySequence(QKeySequence::ZoomIn),
        shortcut_zoom_out		= Qt::CTRL + Qt::Key_Minus,//QKeySequence(QKeySequence::ZoomOut),
        shortcut_panning_left 	= Qt::CTRL + Qt::Key_Left,
        shortcut_panning_right 	= Qt::CTRL + Qt::Key_Right,
        shortcut_panning_up 	= Qt::CTRL + Qt::Key_Up,
        shortcut_panning_down 	= Qt::CTRL + Qt::Key_Down,
		shortcut_change_balance	= Qt::Key_B,

		/*** Rotem ***/
		//Add shortcuts to arrow keys (for some reason keyPressEvent doen't detect arrow keys).
		//////////////////////////////////////////////////////////////////////////////
		shortcut_key_left		= Qt::Key_Left,
		shortcut_key_right		= Qt::Key_Right,
		shortcut_key_up			= Qt::Key_Up,
		shortcut_key_down		= Qt::Key_Down
		//////////////////////////////////////////////////////////////////////////////
		/*** Rotem ***/
    };
//end enum

class MyCvWindow;
class MyViewPort;


class MyGuiReceiver : public QObject
{
    Q_OBJECT

public:
	MyGuiReceiver();
	~MyGuiReceiver();

    int start();
    void isLastWindow();

    bool bTimeOut;
    QTimer* timer;

public slots:
    void createWindow( QString name, int flags = 0 );
    void destroyWindow(QString name);
    void destroyAllWindow();
    void addSlider(QString trackbar_name, QString window_name, void* value, int count, void* on_change);
    void addSlider2(QString trackbar_name, QString window_name, void* value, int count, void* on_change, void *userdata);
    void moveWindow(QString name, int x, int y);
    void resizeWindow(QString name, int width, int height);
    void showImage(QString name, QString title, void* arr);
	void showImage2(QString name, QString title, void* arr, void* arr2);
    void displayInfo( QString name, QString text, int delayms );
    void displayStatusBar( QString name, QString text, int delayms );
    void timeOut();
    void toggleFullScreen(QString name, double flags );
    CvRect getWindowRect(QString name);
    double isFullScreen(QString name);
    double getPropWindow(QString name);
    void setPropWindow(QString name, double flags );
    void setWindowTitle(QString name, QString title);
    double getWindowVisible(QString name);
    double getRatioWindow(QString name);
    void setRatioWindow(QString name, double arg2 );
    void saveWindowParameters(QString name);
    void loadWindowParameters(QString name);
    void putText(void* arg1, QString text, QPoint org, void* font);
    void addButton(QString button_name, int button_type, int initial_button_state , void* on_change, void* userdata);
    void enablePropertiesButtonEachWindow();

    void setOpenGlDrawCallback(QString name, void* callback, void* userdata);
    void setOpenGlContext(QString name);
    void updateWindow(QString name);
    double isOpenGl(QString name);


    //*** Rotem ***
    ////////////////////////////////////////////////////////////////////////////
    //void onWindowStateChanged(Qt::WindowState state);
    //void changeEvent(QEvent *event);
    ////////////////////////////////////////////////////////////////////////////
    //*** Rotem ***

private:
    int nb_windows;
    bool doesExternalQAppExist;
};


enum typeBar { type_CvTrackbar = 0, type_CvButtonbar = 1 };
class MyCvBar : public QHBoxLayout
{
public:
    typeBar type;
    QString name_bar;
    QPointer<QWidget> myparent;
};


class MyCvButtonbar : public MyCvBar
{
    Q_OBJECT
public:
    MyCvButtonbar(QWidget* arg, QString bar_name);

    void addButton(QString button_name, CvButtonCallback call, void* userdata,  int button_type, int initial_button_state);

private:
    void setLabel();

    QPointer<QLabel> label;
    QPointer<QButtonGroup> group_button;
};


class MyCvPushButton : public QPushButton
{
    Q_OBJECT
public:
    MyCvPushButton(MyCvButtonbar* par, QString button_name, CvButtonCallback call, void* userdata);

private:
    MyCvButtonbar* myparent;
    QString button_name ;
    CvButtonCallback callback;
    void* userdata;

private slots:
    void callCallBack(bool);
};


class MyCvCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    MyCvCheckBox(MyCvButtonbar* par, QString button_name, CvButtonCallback call, void* userdata, int initial_button_state);

private:
    MyCvButtonbar* myparent;
    QString button_name ;
    CvButtonCallback callback;
    void* userdata;

private slots:
    void callCallBack(bool);
};


class MyCvRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    MyCvRadioButton(MyCvButtonbar* par, QString button_name, CvButtonCallback call, void* userdata, int initial_button_state);

private:
    MyCvButtonbar* myparent;
    QString button_name ;
    CvButtonCallback callback;
    void* userdata;

private slots:
    void callCallBack(bool);
};


class MyCvTrackbar :  public MyCvBar
{
    Q_OBJECT
public:
    MyCvTrackbar(MyCvWindow* parent, QString name, int* value, int count, CvTrackbarCallback on_change);
    MyCvTrackbar(MyCvWindow* parent, QString name, int* value, int count, CvTrackbarCallback2 on_change, void* data);

    QPointer<QSlider> slider;

private slots:
    void createDialog();
    void update(int myvalue);

private:
    void setLabel(int myvalue);
    void create(MyCvWindow* arg, QString name, int* value, int count);
    QString createLabel();
    QPointer<QPushButton > label;
    CvTrackbarCallback callback;
    CvTrackbarCallback2 callback2;//look like it is use by python binding
    int* dataSlider;
    void* userdata;
};

//Both are top level window, so that a way to differentiate them.
//if (obj->metaObject ()->className () == "MyCvWindow") does not give me robust result

enum typeWindow { type_CvWindow = 1, type_CvWinProperties = 2 };
class MyCvWinModel : public QWidget
{
public:
typeWindow type;
};


class MyCvWinProperties : public MyCvWinModel
{
    Q_OBJECT
public:
    MyCvWinProperties(QString name, QObject* parent);
    ~MyCvWinProperties();
    QPointer<QBoxLayout> myLayout;

private:
    void closeEvent ( QCloseEvent * e ) CV_OVERRIDE;
    void showEvent ( QShowEvent * event ) CV_OVERRIDE;
    void hideEvent ( QHideEvent * event ) CV_OVERRIDE;
};


class MyDefaultViewPort;    //Cass is defined below /*** Rotem ***/


class MyCvWindow : public MyCvWinModel
{
    Q_OBJECT
public:
    MyCvWindow(QString arg2, int flag = CV_WINDOW_NORMAL);
    ~MyCvWindow();

    void setMouseCallBack(CvMouseCallback m, void* param);

    void writeSettings();
    void readSettings();

    double getRatio();
    void setRatio(int flags);

    CvRect getWindowRect();
    int getPropWindow();
    void setPropWindow(int flags);

    void toggleFullScreen(int flags);

    void updateImage(void* arr);
	void updateImage2(void* arr, void* arr2);

    void displayInfo(QString text, int delayms);
    void displayStatusBar(QString text, int delayms);

    void enablePropertiesButton();

    static MyCvButtonbar* createButtonBar(QString bar_name);

    static void addSlider(MyCvWindow* w, QString name, int* value, int count, CvTrackbarCallback on_change CV_DEFAULT(NULL));
    static void addSlider2(MyCvWindow* w, QString name, int* value, int count, CvTrackbarCallback2 on_change CV_DEFAULT(NULL), void* userdata CV_DEFAULT(0));

    void setOpenGlDrawCallback(CvOpenGlDrawCallback callback, void* userdata);
    void makeCurrentOpenGlContext();
    void updateGl();
    bool isOpenGl();

    void setViewportSize(QSize size);

    //parameters (will be save/load)
    int param_flags;
    int param_gui_mode;
    int param_ratio_mode;

    QPointer<QBoxLayout> myGlobalLayout; //All the widget (toolbar, view, LayoutBar, ...) are attached to it
    QPointer<QBoxLayout> myBarLayout;

    QVector<QAction*> vect_QActions;

    QPointer<QStatusBar> myStatusBar;
    QPointer<QToolBar> myToolBar;
    QPointer<QLabel> myStatusBar_msg;

	wchar_t *wname;	/*** Rotem - kind of patch for passing window name as wchar_t ***/

    /*** Rotem ***/
    //Override Window  "change event".
    ////////////////////////////////////////////////////////////////////////////
    virtual void changeEvent(QEvent *event) CV_OVERRIDE;
    ////////////////////////////////////////////////////////////////////////////
    /*** Rotem ***/

    /*** Rotem ***/
    ////////////////////////////////////////////////////////////////////////////
    bool isMatLargerThanScreenResolution();
    ////////////////////////////////////////////////////////////////////////////
    /*** Rotem ***/

protected:
    virtual void keyPressEvent(QKeyEvent* event) CV_OVERRIDE;

private:

    int mode_display; //opengl or native


    /*** Rotem ***/
    //Hold a pointer to object of type MyDefaultViewPort (inherided from MyViewPort) - we need access to members of MyDefaultViewPort
    ////////////////////////////////////////////////////////////////////////////
    //MyViewPort* myView;
    MyDefaultViewPort* myView = nullptr;
    ////////////////////////////////////////////////////////////////////////////
    /*** Rotem ***/

    QVector<QShortcut*> vect_QShortcuts;

    /*** Rotem ***/
    bool m_is_maximized = false;
    /*** Rotem ***/


    void icvLoadTrackbars(QSettings *settings);
    void icvSaveTrackbars(QSettings *settings);
    void icvLoadControlPanel();
    void icvSaveControlPanel();
    void icvLoadButtonbar(MyCvButtonbar* t,QSettings *settings);
    void icvSaveButtonbar(MyCvButtonbar* t,QSettings *settings);

    void createActions();
    void createShortcuts();
    void createToolBar();
    void createView();
    void createStatusBar();
    void createGlobalLayout();
    void createBarLayout();
    MyCvWinProperties* createParameterWindow();

    void hideTools();
    void showTools();
    QSize getAvailableSize();

private slots:
    void displayPropertiesWin();
};


enum type_mouse_event { mouse_up = 0, mouse_down = 1, mouse_dbclick = 2, mouse_move = 3, mouse_wheel = 4 };
static const int tableMouseButtons[][3]={
    {CV_EVENT_LBUTTONUP, CV_EVENT_RBUTTONUP, CV_EVENT_MBUTTONUP},               //mouse_up
    {CV_EVENT_LBUTTONDOWN, CV_EVENT_RBUTTONDOWN, CV_EVENT_MBUTTONDOWN},         //mouse_down
    {CV_EVENT_LBUTTONDBLCLK, CV_EVENT_RBUTTONDBLCLK, CV_EVENT_MBUTTONDBLCLK},   //mouse_dbclick
    {CV_EVENT_MOUSEMOVE, CV_EVENT_MOUSEMOVE, CV_EVENT_MOUSEMOVE},               //mouse_move
    {0, 0, 0}                                                                   //mouse_wheel, to prevent exceptions in code
};


class MyViewPort
{
public:
    virtual ~MyViewPort() {}

    virtual QWidget* getWidget() = 0;

    virtual void setMouseCallBack(CvMouseCallback callback, void* param) = 0;

    virtual void writeSettings(QSettings& settings) = 0;
    virtual void readSettings(QSettings& settings) = 0;

    virtual double getRatio() = 0;
    virtual void setRatio(int flags) = 0;

    virtual void updateImage(const CvArr* arr) = 0;
	virtual void updateImage2(const CvArr* arr, const CvArr* arr2) = 0; /*** Rotem ***/

    virtual void startDisplayInfo(QString text, int delayms) = 0;

    virtual void setOpenGlDrawCallback(CvOpenGlDrawCallback callback, void* userdata) = 0;
    virtual void makeCurrentOpenGlContext() = 0;
    virtual void updateGl() = 0;

    virtual void setSize(QSize size_) = 0;
};


class MyOCVViewPort : public MyViewPort
{
public:
    explicit MyOCVViewPort();
    ~MyOCVViewPort() CV_OVERRIDE {};
    void setMouseCallBack(CvMouseCallback callback, void* param) CV_OVERRIDE;

protected:
    void icvmouseEvent(QMouseEvent* event, type_mouse_event category);
    void icvmouseHandler(QMouseEvent* event, type_mouse_event category, int& cv_event, int& flags);
    virtual void icvmouseProcessing(QPointF pt, int cv_event, int flags);

    CvMouseCallback mouseCallback;
    void* mouseData;
};


#ifdef HAVE_QT_OPENGL

class MyOpenGlViewPort : public QGLWidget, public MyOCVViewPort
{
public:
    explicit MyOpenGlViewPort(QWidget* parent);
    ~MyOpenGlViewPort() CV_OVERRIDE;

    QWidget* getWidget() CV_OVERRIDE;

    void writeSettings(QSettings& settings) CV_OVERRIDE;
    void readSettings(QSettings& settings) CV_OVERRIDE;

    double getRatio() CV_OVERRIDE;
    void setRatio(int flags) CV_OVERRIDE;

    void updateImage(const CvArr* arr) CV_OVERRIDE;

	void updateImage2(const CvArr* arr, const CvArr* arr2) CV_OVERRIDE; /*** Rotem ***/

    void startDisplayInfo(QString text, int delayms) CV_OVERRIDE;

    void setOpenGlDrawCallback(CvOpenGlDrawCallback callback, void* userdata) CV_OVERRIDE;
    void makeCurrentOpenGlContext() CV_OVERRIDE;
    void updateGl() CV_OVERRIDE;

    void setSize(QSize size_) CV_OVERRIDE;

protected:
    void initializeGL() CV_OVERRIDE;
    void resizeGL(int w, int h) CV_OVERRIDE;
    void paintGL() CV_OVERRIDE;

    void wheelEvent(QWheelEvent* event) CV_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) CV_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) CV_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) CV_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event) CV_OVERRIDE;

    QSize sizeHint() const CV_OVERRIDE;

private:
    QSize size;

    CvOpenGlDrawCallback glDrawCallback;
    void* glDrawData;
};

#endif // HAVE_QT_OPENGL


class MyDefaultViewPort : public QGraphicsView, public MyOCVViewPort
{
    Q_OBJECT

public:
    MyDefaultViewPort(MyCvWindow* centralWidget, int arg2);
    ~MyDefaultViewPort() CV_OVERRIDE;

    QWidget* getWidget() CV_OVERRIDE;

    void writeSettings(QSettings& settings) CV_OVERRIDE;
    void readSettings(QSettings& settings) CV_OVERRIDE;

    double getRatio() CV_OVERRIDE;
    void setRatio(int flags) CV_OVERRIDE;

    void updateImage(const CvArr* arr) CV_OVERRIDE;

	void updateImage2(const CvArr* arr, const CvArr* arr2) CV_OVERRIDE; /*** Rotem ***/

    void startDisplayInfo(QString text, int delayms) CV_OVERRIDE;

    void setOpenGlDrawCallback(CvOpenGlDrawCallback callback, void* userdata) CV_OVERRIDE;
    void makeCurrentOpenGlContext() CV_OVERRIDE;
    void updateGl() CV_OVERRIDE;

    void setSize(QSize size_) CV_OVERRIDE;

public slots:
    //reference:
    //http://www.qtcentre.org/wiki/index.php?title=QGraphicsView:_Smooth_Panning_and_Zooming
    //http://doc.qt.nokia.com/4.6/gestures-imagegestures-imagewidget-cpp.html

    void siftWindowOnLeft();
    void siftWindowOnRight();
    void siftWindowOnUp() ;
    void siftWindowOnDown();

    void resetZoom();
    void imgRegion();
    void ZoomIn();
    void ZoomOut();

    void saveView();

	/*** Rotem ***/
	//Events for arrow keys (for some reason keyPressEvent doesn't detect arrow keys).
	//////////////////////////////////////////////////////////////////////////////
	void leftArrowPressed();
	void rightArrowPressed();
	void upArrowPressed();
	void downArrowPressed();

	//Slots for forward and backward toolbar icons.
	void previousImage();
	void nextImage();

	//Slot for "balance" changing.
	void changeBalance();
	//////////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/


protected:
    void contextMenuEvent(QContextMenuEvent* event) CV_OVERRIDE;
    void resizeEvent(QResizeEvent* event) CV_OVERRIDE;
    void paintEvent(QPaintEvent* paintEventInfo) CV_OVERRIDE;

    void wheelEvent(QWheelEvent* event) CV_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) CV_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) CV_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) CV_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event) CV_OVERRIDE;

private:
    int param_keepRatio;

    //parameters (will be save/load)
    QTransform param_matrixWorld;

    CvMat* image2Draw_mat = nullptr;
    QImage image2Draw_qt;
    int nbChannelOriginImage;

	/*** Rotem ***/
	//Holds image data before processing for display (i.e. 16 bits image).
	//////////////////////////////////////////////////////////////////////////
	CvMat* image2Draw_mat2 = nullptr;
	QImage image2Draw_qt2;
	int nbChannelOriginImage2;    
	//////////////////////////////////////////////////////////////////////////
	/*** Rotem ***/



    void scaleView(qreal scaleFactor, QPointF center);
    void moveView(QPointF delta);

    QPoint  mouseCoordinate;
    QPointF positionGrabbing;
    QRect   positionCorners;
    QTransform matrixWorld_inv;
    float ratioX, ratioY;

    bool isSameSize(IplImage* img1,IplImage* img2);

    QSize sizeHint() const CV_OVERRIDE;
    QPointer<MyCvWindow> centralWidget;
    QPointer<QTimer> timerDisplay;
    bool drawInfo;
    QString infoText;
    QRectF target;

    void drawInstructions(QPainter *painter);
    void drawViewOverview(QPainter *painter);
    void drawImgRegion(QPainter *painter);
    void draw2D(QPainter *painter);
    void drawStatusBar();
    void controlImagePosition();

    void icvmouseProcessing(QPointF pt, int cv_event, int flags) CV_OVERRIDE;

private slots:
    void stopDisplayInfo();

public:
	/*** Rotem ***/
	//////////////////////////////////////////////////////////////////////////
    CvMat* getImage2Draw_mat2() {return image2Draw_mat2;}
    //////////////////////////////////////////////////////////////////////////
    /*** Rotem ***/
};



MY_CV_IMPL CvFont mycvFontQt(const char* nameFont, int pointSize, CvScalar color, int weight, int style, int spacing);
MY_CV_IMPL void mycvAddText(const CvArr* img, const char* text, CvPoint org, CvFont* font);
MY_CV_IMPL void mycvDisplayOverlay(const wchar_t* name, const char* text, int delayms);
MY_CV_IMPL void mycvSaveWindowParameters(const wchar_t* name);
MY_CV_IMPL void mycvLoadWindowParameters(const wchar_t* name);
MY_CV_IMPL void mycvDisplayStatusBar(const wchar_t* name, const char* text, int delayms);
MY_CV_IMPL int mycvWaitKey(int delay);
MY_CV_IMPL int mycvStartLoop(int(*pt2Func)(int argc, char *argv[]), int argc, char* argv[]);
MY_CV_IMPL void mycvStopLoop();
MY_CV_IMPL int mycvInitSystem(int, char**);
MY_CV_IMPL int mycvNamedWindow(const wchar_t* name, const wchar_t* title, int flags = CV_WINDOW_AUTOSIZE);
MY_CV_IMPL void mycvDestroyWindow(const wchar_t* name);
MY_CV_IMPL void mycvDestroyAllWindows();
MY_CV_IMPL void* mycvGetWindowHandle(const wchar_t* name);
MY_CV_IMPL const wchar_t* mycvGetWindowName(void* window_handle);
MY_CV_IMPL void mycvMoveWindow(const wchar_t* name, int x, int y);
MY_CV_IMPL void mycvResizeWindow(const wchar_t* name, int width, int height);
MY_CV_IMPL int mycvCreateTrackbar2(const wchar_t* name_bar, const wchar_t* window_name, int* val, int count, CvTrackbarCallback2 on_notify, void* userdata);
MY_CV_IMPL int mycvStartWindowThread();
MY_CV_IMPL int mycvCreateTrackbar(const wchar_t* name_bar, const wchar_t* window_name, int* value, int count, CvTrackbarCallback on_change);
MY_CV_IMPL int mycvCreateButton(const char* button_name, CvButtonCallback on_change, void* userdata, int button_type, int initial_button_state);
MY_CV_IMPL int mycvGetTrackbarPos(const wchar_t* name_bar, const wchar_t* window_name);
MY_CV_IMPL void mycvSetTrackbarPos(const wchar_t* name_bar, const wchar_t* window_name, int pos);
MY_CV_IMPL void mycvSetTrackbarMax(const wchar_t* name_bar, const wchar_t* window_name, int maxval);
MY_CV_IMPL void mycvSetTrackbarMin(const wchar_t* name_bar, const wchar_t* window_name, int minval);
MY_CV_IMPL void mycvSetMouseCallback(const wchar_t* window_name, CvMouseCallback on_mouse, void* param);
MY_CV_IMPL void mycvShowImage(const wchar_t* name, const wchar_t* title, const CvArr* arr);
MY_CV_IMPL void mycvShowImage2(const wchar_t* name, const wchar_t* title, const CvArr* arr, const CvArr* arr2);
MY_CV_IMPL void mycvSetBalanceIcon(const wchar_t* name, int balance_select); /*** Rotem ***/
MY_CV_IMPL void mycvSetOpenGlDrawCallback(const wchar_t* window_name, CvOpenGlDrawCallback callback, void* userdata);

#ifdef HAVE_QT_OPENGL
MY_CV_IMPL void mycvSetOpenGlContext(const wchar_t* window_name);
MY_CV_IMPL void mycvSetOpenGlContext(const wchar_t* window_name);
MY_CV_IMPL void mycvUpdateWindow(const wchar_t* window_name);
#endif





#endif
