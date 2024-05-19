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
 The processing is required for overcoming the issue that most 16bpc images don't cover the full range of [0, 2<sup>16</sup>-1], and are usually very dark when displayed without processing.
 - The viewer enables zooming and inspection features that are part of OpenCV `imshow` functionality when OpenCV is built with [Qt](https://en.wikipedia.org/wiki/Qt_(software)).
 - The viewer inspects the original pixel values before processing (not the 8bpc values that are displayed on the screen).  
 - The viewer allows skipping forward and backward to the next and previous image with the same file extension.
 - The "Open Source" nature of the project, allows incorporating customized features.
 
Inspecting pixel value with mouse cursor:   
![alt text](https://github.com/cohenrotem/WayView/blob/main/Screenshots/PixelInspection.png)

Inspecting pixel values when zooming in:  
![alt text](https://github.com/cohenrotem/WayView/blob/main/Screenshots/ZoomX30.png)


## Getting Started

### Dependencies

 - WayView.exe is a standalone Windows 64-bit application (tested in Windows 10).
 - Using WayView in Linux is supported, but required building the application from source.  
  In Linux, WayView executable requires installation of Qt5. 

## Installing

### Windows installation

 - Download the latest release of WayView.exe from [releases](https://github.com/cohenrotem/WayView/releases).
 - Copy WayView.exe to know folder such as "c:\Program Files\WayView".  
 (Create a folder named WayView in "c:\Program Files" folder, and copy WayView.exe to the created folder).  


### Linux installation

In the current state of the project, the Linux installation requires building from source.  
Note: The g++ compiler must support C++17.  
The following build instructions were tested with Ubuntu 20.04:  

 - Install Qt5 (if required):  
 `sudo apt update`  
 `sudo apt install qtbase5-dev`
	
We are going to build statically linked OpenCV version.  
The statically linked OpenCV is going to be linked dynamically to the (non-static) shared objects of Qt5.

Building OpenCV from source:  
 - Open command line terminal.
 - Change directory to home (for example):  
 `cd ~`
 
 - Install minimal prerequisites  (https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)  
 `sudo apt update && sudo apt install -y cmake g++ wget unzip`

 - Download and unpack sources of OpenCV 3.4.16  
 `wget -O opencv.zip https://github.com/opencv/opencv/archive/3.4.16.zip`  
 `unzip opencv.zip`

 - Create build directory  
 `cd opencv-3.4.16 && mkdir -p build && cd build`
 
 - Configure  
 `cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/opencv-3.4.16 -DBUILD_CUDA_STUBS=OFF -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_FAT_JAVA_LIB=OFF -DBUILD_IPP_IW=ON -DBUILD_ITT=OFF -DBUILD_JASPER=ON -DBUILD_JAVA=OFF -DBUILD_JPEG=ON -DBUILD_LIST=OFF -DBUILD_OPENEXR=ON -DBUILD_PACKAGE=ON -DBUILD_PERF_TESTS=OFF -DBUILD_PNG=ON -DBUILD_PROTOBUF=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_TBB=OFF -DBUILD_TESTS=OFF -DBUILD_TIFF=ON -DBUILD_USE_SYMLINKS=OFF -DBUILD_WEBP=ON -DBUILD_WITH_DEBUG_INFO=ON -DBUILD_WITH_DYNAMIC_IPP=OFF -DBUILD_WITH_STATIC_CRT=ON -DBUILD_ZLIB=ON -DBUILD_opencv_apps=OFF -DBUILD_opencv_calib3d=OFF -DBUILD_opencv_core=ON -DBUILD_opencv_dnn=OFF -DBUILD_opencv_features2d=OFF -DBUILD_opencv_flann=OFF -DBUILD_opencv_highgui=ON -DBUILD_opencv_imgcodecs=ON -DBUILD_opencv_imgproc=ON -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_js=OFF -DBUILD_opencv_js_bindings_generator=OFF -DBUILD_opencv_ml=OFF -DBUILD_opencv_objdetect=OFF -DBUILD_opencv_photo=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_opencv_shape=OFF -DBUILD_opencv_stitching=OFF -DBUILD_opencv_superres=OFF -DBUILD_opencv_ts=OFF -DBUILD_opencv_video=OFF -DBUILD_opencv_videoio=OFF -DBUILD_opencv_videostab=OFF -DBUILD_opencv_world=OFF -DWITH_1394=OFF -DWITH_ARITH_DEC=OFF -DWITH_ARITH_ENC=OFF -DWITH_CLP=OFF -DWITH_CSTRIPES=OFF -DWITH_CUDA=OFF -DWITH_DIRECTX=OFF -DWITH_DSHOW=OFF -DWITH_EIGEN=OFF -DWITH_FFMPEG=OFF -DWITH_GDAL=OFF -DWITH_GDCM=OFF -DWITH_GIGEAPI=OFF -DWITH_GSTREAMER=OFF -DWITH_GSTREAMER_0_10=OFF -DWITH_HALIDE=OFF -DWITH_IMGCODEC_HDR=ON -DWITH_IMGCODEC_PXM=ON -DWITH_IMGCODEC_SUNRASTER=ON -DWITH_INF_ENGINE=OFF -DWITH_INTELPERC=OFF -DWITH_IPP=OFF -DWITH_ITT=OFF -DWITH_JASPER=ON -DWITH_JPEG=ON -DWITH_LAPACK=ON -DWITH_MFX=OFF -DWITH_MSMF=OFF -DWITH_MSMF_DXVA=OFF -DWITH_NGRAPH=OFF -DWITH_OPENCL=OFF -DWITH_OPENCLAMDBLAS=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCL_D3D11_NV=OFF -DWITH_OPENCL_SVM=OFF -DWITH_OPENEXR=ON -DWITH_OPENGL=OFF -DWITH_OPENMP=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_OPENVX=OFF -DWITH_PNG=ON -DWITH_PROTOBUF=OFF -DWITH_PVAPI=OFF -DWITH_QT=ON -DWITH_QUIRC=OFF -DWITH_TBB=OFF -DWITH_TIFF=ON -DWITH_VFW=OFF -DWITH_VTK=OFF -DWITH_WEBP=ON -DWITH_WIN32UI=OFF -DWITH_XIMEA=OFF -DCPU_DISPATCH=SSE3 ..`

 - Build  
 `make -j4`

 - Install (to /opt/opencv-3.4.16)  
 `sudo make install`

Building WayView from source:  
 - Change directory to home (for example), make directory named projects (if required)  
 `cd ~`  
 `mkdir -p projects && cd projects`

 - Clone the repository of WayView (assume latest commit is desired, and assume git is installed)  
 `git clone https://github.com/cohenrotem/WayView.git`

 - Create build directory  
 `cd WayView/WayView && mkdir -p build && cd build`

 - Configure  
 `cmake ..`

 - Build  
 `make -j4`
 
 - Copy WayView executable to /opt/WayView  
  `sudo mkdir -p /opt/WayView`
  `sudo cp ~/projects/WayView/WayView/build/bin/WayView /opt/WayView`


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

## Building WayView from Source using Microsoft Visual Studio

The last commit requires a compiler that supports C++17.  
WayView is statically linked with OpenCV and Qt5.  
For statically link OpenCV and Qt5, we have to build OpenCV and Qt5 from source using Visual Studio Platform Toolset (build all with the same version of MSVC).  
OpenCV depends on Qt5, so we have to build Qt5 before building OpenCV.  
Start by downloading the sources of OpenCV 3.4.16 and the sources of Qt5.  

Building Qt5 from source using Visual Studio 2019 (instructions are incomplete):  
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
     BUILD_SHARED_LIBS=OFF BUILD_TESTS=OFF BUILD_WITH_DEBUG_INFO=ON BUILD_opencv_calib3d=OFF BUILD_opencv_core=ON BUILD_opencv_dnn=OFF BUILD_opencv_features2d=OFF BUILD_opencv_flann=OFF BUILD_opencv_highgui=ON BUILD_opencv_imgcodecs=ON BUILD_opencv_imgproc=ON BUILD_opencv_java_bindings_generator=OFF BUILD_opencv_js=OFF BUILD_opencv_js_bindings_generator=OFF BUILD_opencv_ml=OFF BUILD_opencv_objdetect=OFF BUILD_opencv_photo=ON BUILD_opencv_python2=OFF BUILD_opencv_python3=OFF BUILD_opencv_python_bindings_generator=OFF BUILD_opencv_python_tests=OFF BUILD_opencv_shape=OFF BUILD_opencv_stitching=OFF BUILD_opencv_superres=OFF BUILD_opencv_ts=OFF BUILD_opencv_video=OFF BUILD_opencv_videoio=OFF BUILD_opencv_videostab=OFF BUILD_opencv_world=OFF CPU_DISPATCH=SSE3 OPENCV_DNN_OPENCL=OFF WITH_1394=OFF WITH_ARITH_DEC=OFF WITH_ARITH_ENC=OFF WITH_DIRECTX=OFF WITH_DSHOW=OFF WITH_EIGEN=OFF WITH_FFMPEG=OFF WITH_GSTREAMER=OFF WITH_ITT=OFF WITH_MSMF=OFF WITH_MSMF_DXVA=OFF WITH_OPENCL=OFF WITH_OPENCLAMDBLAS=OFF WITH_OPENCLAMDFFT=OFF WITH_OPENCL_D3D11_NV=OFF WITH_QUIRC=OFF WITH_IPP=OFF WITH_QT=ON Qt5_DIR="c:/qt5-build/qtbase/lib/cmake/Qt5"
     ```
 - It is recommended to build OpenCV twice: With Debug configuration, and with Release configuration.  
 - Defined Windows system environment variables:  
     ```
     QT_DIR = c:\qt5-build\install\Qt\Qt-5.12.1
     OPENCVQT_DIR = c:\opencv-3.4.16\build\install
     ```
 - Use cmake-gui or command line for setting up MCVC solution from CMakeLists.txt  
 Open WayView.sln in Visual Studio, select configuration (Debug or Release) and build the project.  
 Note: Building was tested with Visual Studio 2019 only.  
 Note: The Debug configuration EXE is named WayViewd.exe (WayViewd.exe is not part of the official release).  

## Building WayView from Source in Linux

The last commit uses CMake, and supports Linux build.  
The Linux build is statically linked with OpenCV, but dynamically linked with Qt5.  
Refer to "Linux installation" section for details.  

## Authors

Rotem C.

## Version History

 - 0.1
    - Initial Release
 - 0.1c
    - Use CMake and support Linux (not yet released)

## License

This project is licensed under the GPL v3 License - see the LICENSE.md file for details.  
The license must comply Qt5 5.12.1 license, when the executable is statically linked with Qt5.  
The license must also comply OpenCV-3.4.16 license.  
WayView.exe is built using Visual Studio 2019 Community Edition, and applies the following [license terms](https://visualstudio.microsoft.com/license-terms/mlt031819/).

## Acknowledgments

 - [https://github.com/opencv/opencv](https://github.com/opencv/opencv).  
 - [https://contribute.qt-project.org/](https://contribute.qt-project.org/).
