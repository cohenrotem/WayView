cmake_minimum_required(VERSION 3.0)

# We may select Debug or Release configuration here, or remove this line, and add -DCMAKE_BUILD_TYPE=value to CMake command line.
#set(CMAKE_BUILD_TYPE Debug CACHE STRING "select build type to Debug or Release")

# The following definitions my be modified by the user - adjusting definitions for the specific machine
# We define the OpenCV and Qt installation directories (we may also use system environment variables)

if (UNIX)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "select build type to Debug or Release")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug)

    # Assume OpenCV installation folder is: /opt/opencv-3.4.16
    set(OpenCV_DIR /opt/opencv-3.4.16 CACHE PATH "OpenCV installation path")    
else()
    # If Windows:	
    # Example: OpenCV_DIR = D:\opencv-3.4.16\build\install\x64\vc16\staticlib
    set(OpenCV_DIR $ENV{OPENCVQT_DIR}/x64/vc16/staticlib CACHE PATH "OpenCV installation path")
    
    # Example: Qt5_DIR = D:\qt5-build\install\Qt\Qt-5.12.1\lib\cmake\Qt5
    set(Qt5_DIR $ENV{QT_DIR}/lib/cmake/Qt5 CACHE PATH "Qt5 installation path")
endif()
