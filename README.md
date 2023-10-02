# WayView

**WayView** is an image viewer based on [OpenCV](https://opencv.org/) [imshow](https://docs.opencv.org/4.x/d7/dfc/group__highgui.html#ga453d42fe4cb60e5723281a89973ee563) function.  
The main purpose is viewing and inspecting RGB images with 16 bits per component.  
The software is meant to be used as an "engineering tool", for viewing and inspecting "problematic" images.

 - Note: The project is an incomplete "Demo".

## Description

Main usage is right clicking an image file using Windows File Explorer, pressing Open with and selecting WayView.exe  

Advantages of WayView:  
 - WayView.exe is built as a standalone Windows application without external DLL dependencies (the dependent libraries are statically linked).  
 - When opening a 16bpc image, the image is automatically processed for display.  
 The current version applies "linear stretching" before displaying the image (only for 16bpc images).  
 The processing is required for overcoming the issue that most 16bpc images don't cover the full range of [0, 2^16^-1], and are usually very dark when displayed without processing.
 - The viewer enables zooming and inspection features that are part of OpenCV `imshow` functionality when OpenCV is built with [Qt](https://en.wikipedia.org/wiki/Qt_(software)).
 - The viewer inspects the original pixel values before processing (not the 8bpc values that are displayed on the screen).  
 - The viewer allows skipping forward and backward to the next and previous image with the same file extension.
 - The "Open Source" nature of the project, allows incorporating customized features.


## Getting Started

### Dependencies

 - WayView.exe is a standalone Windows 64-bit application (tested in Windows 10).

### Installing

 - How/where to download your program
 - Download WayView.exe and copy the file to know folder such as "c:\Program Files\WayView".  
 (Create a folder named WayView in "c:\Program Files" folder, and copy WayView.exe to the created folder).  


### Executing program

 - Executing WayView from command line:  
 `"c:\Program Files\WayView\WayView.exe" [imagefilename]`  
 
The recommended usage is using the image file Context Menu.  
Assume WayView.exe is in "c:\Program Files\WayView" folder.  
For the first time execution, it's recommended to use the following steps:  
 - Right an image file in Windows File Explorer (context menu is opened).
 - In the context menu, select "Open with".
 - Remove the V from "Always use this app to open..." if the check-box appears.
 - Select "Choose another app" then "Look for another app on this PC", and browse for "c:\Program Files\WayView\WayView.exe" (required at the first time).  
 After the first time, WayView is going to be in one of the lists for selection.  

## Help

In addition to the existing functionality of imshow (given OpenCV is built with Qt), there are the following additional functionality:
 - Left and Right arrow keys (and buttons) skip to the next/previous image with the same file extension.
 - Press Esc for exit.
 - B0 button functionality is not yet implemented (reserved for 4 display presets).

## Building WayView from Source using Microsoft Visual Studio 2019
In the current state of the project, WayView is built in Visual Studio 2019 IDE (there are no CMake files).  
WayView is statically linked with OpenCV and Qt5.  
For statically link OpenCV and Qt5, we have to build OpenCV and Qt5 from source using Visual Studio 2019 Platform Toolset.  
OpenCV depends on Qt5, so we have to build Qt5 before building OpenCV.  
Start by downloading the sources of OpenCV 3.4.16 and the sources of Qt5.  

Building Qt5 from source (instructions are incomplete):  
 - Install Python 3 and Perl if not installed (may install [Strawberry Perl](https://strawberryperl.com/)).  
 Make sure python.exe and perl.exe are in the system path, when building Qt5 from source (Qt5 build scripts requires both Python and Perl).  
 - Download Qt5 sources as described [here](https://doc.qt.io/qt-5/windows-building.html) to C:\Qt5 folder.
 - Checkout version Qt5.12.1
 - Set up Microsoft Visual Studio 2019 compiler where <arch> is amd64 (from command line):  
    ```
    "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
    SET _ROOT=C:\Qt5
    SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;%PATH%
    SET PATH=%_ROOT%\qtrepotools\bin;%PATH%
    SET _ROOT=
    ```
 - Make folder, configure and build and install:  
    ```
    mkdir qt5-build
    cd qt5-build
    ..\qt5\configure --debug-and-release -static -static-runtime -opengl desktop -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype -nomake examples -nomake tests -skip qtwebengine -opensource
    nmake
    nmake install
    ```
 - Move the folder C:\Qt to C:\qt5-build\install
 
Building OpenCV from source (instructions are incomplete):
 - Download OpenCV 3.4.16 to C:\opencv-3.4.16
 - Build OpenCV using CMake with the following configuration:  
     ```
     BUILD_SHARED_LIBS=OFF BUILD_TESTS=OFF BUILD_WITH_DEBUG_INFO=ON BUILD_opencv_calib3d=OFF BUILD_opencv_core=ON BUILD_opencv_dnn=OFF BUILD_opencv_features2d=OFF BUILD_opencv_flann=OFF BUILD_opencv_highgui=ON BUILD_opencv_imgcodecs=ON BUILD_opencv_imgproc=ON BUILD_opencv_java_bindings_generator=OFF BUILD_opencv_js=OFF BUILD_opencv_js_bindings_generator=OFF BUILD_opencv_ml=OFF BUILD_opencv_objdetect=OFF BUILD_opencv_photo=ON BUILD_opencv_python2=OFF BUILD_opencv_python3=OFF BUILD_opencv_python_bindings_generator=OFF BUILD_opencv_python_tests=OFF BUILD_opencv_shape=OFF BUILD_opencv_stitching=OFF BUILD_opencv_superres=OFF BUILD_opencv_ts=OFF BUILD_opencv_video=OFF BUILD_opencv_videoio=OFF BUILD_opencv_videostab=OFF BUILD_opencv_world=ON CPU_DISPATCH=SSE3 OPENCV_DNN_OPENCL=OFF WITH_1394=OFF WITH_ARITH_DEC=OFF WITH_ARITH_ENC=OFF WITH_DIRECTX=OFF WITH_DSHOW=OFF WITH_EIGEN=OFF WITH_FFMPEG=OFF WITH_GSTREAMER=OFF WITH_ITT=OFF WITH_MSMF=OFF WITH_MSMF_DXVA=OFF WITH_OPENCL=OFF WITH_OPENCLAMDBLAS=OFF WITH_OPENCLAMDFFT=OFF WITH_OPENCL_D3D11_NV=OFF WITH_QUIRC=OFF WITH_IPP=OFF WITH_QT=ON Qt5_DIR="c:/qt5-build/qtbase/lib/cmake/Qt5"
     ```
 - Defined Windows system environment variables:  
     ```
     QT_DIR = c:\qt5-build\install\Qt\Qt-5.12.1
     OPENCVQT_DIR = c:\opencv-3.4.16\build\install
     ```
 - Open WayView.sln in Visual Studio 2019, select configuration (Debug or Release) and build the project.  
 Note: building the project (and dependent libraries) using more advanced version of Visual Studio should work, but not tested.

## Authors

Rotem C.

## Version History

 - 0.1
    - Initial Release

## License

This project is licensed under the GPL v3 License - see the LICENSE.md file for details.  
The license must comply Qt5 5.12.1 license, when the executable is statically linked with Qt5.  
The license must also comply OpenCV-3.4.16 license.  
WayView.exe is built using Visual Studio 2019 Community Edition, and applies the following [license terms](https://visualstudio.microsoft.com/license-terms/mlt031819/).

## Acknowledgments

 - [https://github.com/opencv/opencv](https://github.com/opencv/opencv).  
 - [https://contribute.qt-project.org/](https://contribute.qt-project.org/).
