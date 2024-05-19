cmake_minimum_required(VERSION 3.0)

project(WayView)

# project_defaults.cmake adds 
include(project_local_deffinitions.cmake)

#https://github.com/IFL-CAMP/simple/blob/master/CMakeLists.txt
# Set the project version.
set(PROJECT_VER_MAJOR 0)
set(PROJECT_VER_MINOR 1)
set(PROJECT_VER_PATCH "c")
set(WayView_VERSION "${PROJECT_VER_MAJOR}.${PROJECT_VER_MINOR}${PROJECT_VER_PATCH}")

# Here all the compiler options for all projects created are set.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_POSTFIX "d")

# Output all binaries and libs into a common folder across all libraries.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Keep only Debug and Release configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported configuration types" FORCE)

# Select static link of OpenCV
# https://stackoverflow.com/questions/3755482/how-do-i-tell-cmake-i-want-my-project-to-link-libraries-statically
set(OpenCV_STATIC ON)
# https://stackoverflow.com/questions/68122596/cmake-statically-link-against-opencv-libs
set(OpenCV_SHARED OFF)

# Add OpenCV to project (OpenCV_DIR is defined in project_local_deffinitions.cmake)
find_package(OpenCV REQUIRED)

# https://doc.qt.io/qt-5/cmake-get-started.html
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# Add Qt5 to project (OpenCV_DIR is defined in project_local_deffinitions.cmake)
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Test
    Concurrent
)

# Collect *.cpp and *.h to SOURCES variable
file(GLOB SOURCES
    *.h
    *.cpp
)

add_compile_definitions(PROJECT_VER_MAJOR=${PROJECT_VER_MAJOR})
add_compile_definitions(PROJECT_VER_MINOR=${PROJECT_VER_MINOR})
add_compile_definitions(PROJECT_VER_PATCH=${PROJECT_VER_PATCH})
add_compile_definitions(WAYVIEW_VERSION="${WayView_VERSION}")

if(MSVC)
    add_compile_definitions(WIN32)
    add_compile_definitions(_WINDOWS)
    add_compile_definitions(_UNICODE)
    add_compile_definitions(UNICODE)  
          
    add_executable(WayView
        ${SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/moc_my_window_QT.cpp
        WayView.rc
        WayView.ico
        )
    
    #https://gist.github.com/sonictk/8ce00c6019d3a19056c054e485326434
    set(MSVC_COMPILE_OPTIONS "/W4;/Gy;/Zc:wchar_t;/nologo;/permissive-")
    set(MSVC_COMPILE_DEBUG_OPTIONS "${MSVC_COMPILE_OPTIONS};/ZI;/Od;/MTd")
    set(MSVC_COMPILE_RELEASE_OPTIONS "${MSVC_COMPILE_OPTIONS};/Zi;/O2;/Oi;/MT")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Debug>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:${MSVC_COMPILE_RELEASE_OPTIONS}>")
          
    target_link_options(WayView PRIVATE "/INCREMENTAL;/DEBUG:FULL;/SUBSYSTEM:WINDOWS")
elseif(UNIX)
    add_executable(WayView
        ${SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/moc_my_window_QT.cpp
        )
else()
    message(SEND_ERROR "Unsupported configuration - current support is either MSVC in Windows or Linux")
endif()

# Generate Qt MOC file
qt5_generate_moc(my_window_QT.h ${CMAKE_CURRENT_BINARY_DIR}/moc_my_window_QT.cpp TARGET WayView)


# Add OpenCV additional include directories.
include_directories(${OpenCV_INCLUDE_DIRS})

# Add Qt5 additional include directories.
include_directories(${Qt5Core_INCLUDE_DIRS})

# Add OpenCV libraries.
target_link_libraries(WayView ${OpenCV_LIBS})

# Add Qt5 libraries. https://doc.qt.io/qt-5/cmake-get-started.html
#set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE}")  # Maybe required in Linux?
# set up a mapping so that the Release configuration for the Qt imported target is
# used in the COVERAGE CMake configuration.
#set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_COVERAGE "RELEASE") # Maybe required in Linux?
target_link_libraries(WayView
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    )

if(MSVC)
    # Set QT_DIR to the folder of Qt cmake files
    set(QT_DIR "${Qt5_DIR}/..")
    
    # https://blog.pcitron.fr/2019/10/14/linking-against-a-static-qt5-build-using-cmake/
    # Note: the blog content was edited (for passing the linking stage) - the edits might not be correctly orgenized.

    #
    # Set a few paths
    #    
    set (QT_LIB_DIR "${QT_DIR}/..")
    set (QT_PLUGIN_DIR "${QT_DIR}/../../plugins")

    #
    # Qt5::QWindowsIntegrationPlugin
    # note that this target somehow is already there even if we didn't search for it in the
    # find_package command. And since it's mandatory to run the most basic Widgets application
    # on Windows ...
    #
    if (TARGET Qt5::QWindowsIntegrationPlugin)
        # find additional components needed by the windows platform plugin
        #find_package (Qt5 5
        #    COMPONENTS
        #        EventDispatcherSupport
        #        FontDatabaseSupport
        #        ThemeSupport
        #    REQUIRED
        #)

        # configure direct dependencies of the plugin
        target_link_libraries(Qt5::QWindowsIntegrationPlugin
            INTERFACE
                # Qt targets
                #Qt5::EventDispatcherSupport
                #Qt5::FontDatabaseSupport
                #Qt5::ThemeSupport
                # Windows libs
                Dwmapi.lib
                Imm32.lib
                Wtsapi32.lib
                # The following is needed if you want to use the Windows vista style plugin.
                # If you provide your own style or CSS, you can comment out the following libs.
                $<$<CONFIG:Debug>:${QT_PLUGIN_DIR}/styles/qwindowsvistastyled.lib>
                $<$<CONFIG:Release>:${QT_PLUGIN_DIR}/styles/qwindowsvistastyle.lib>
                UxTheme.lib
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/Qt5ThemeSupportd.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/Qt5ThemeSupport.lib>
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/Qt5WindowsUIAutomationSupportd.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/Qt5WindowsUIAutomationSupport.lib>
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/Qt5EventDispatcherSupportd.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/Qt5EventDispatcherSupport.lib>
        )
    endif ()

    #
    # Qt5::FontDatabaseSupport
    #
    if (TARGET Qt5::FontDatabaseSupport)
        target_link_libraries(Qt5::FontDatabaseSupport
            INTERFACE
                # Qt libs
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/qtfreetyped.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/qtfreetype.lib>
        )
    else ()
        # Add it to Qt5::Core, because we need to link these lib files 
        target_link_libraries(Qt5::Core
            INTERFACE
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/Qt5FontDatabaseSupportd.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/Qt5FontDatabaseSupport.lib>
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/qtfreetyped.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/qtfreetype.lib>
        )
    endif ()

    #
    # Qt5::Gui
    #
    if (TARGET Qt5::Gui)
        target_link_libraries(Qt5::Gui
            INTERFACE
                # Qt targets
                Qt5::QWindowsIntegrationPlugin
                # Qt libs
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/qtlibpngd.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/qtlibpng.lib>
        )
    endif ()

    #
    # Qt5::Core
    #
    if (TARGET Qt5::Core)
        target_link_libraries(Qt5::Core
            INTERFACE
                # Qt libs
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/qtpcre2d.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/qtpcre2.lib>
                $<$<CONFIG:Debug>:${QT_LIB_DIR}/qtharfbuzzd.lib>
                $<$<CONFIG:Release>:${QT_LIB_DIR}/qtharfbuzz.lib>
                # Windows libs
                Netapi32.lib
                Ws2_32.lib
                UserEnv.lib
                Version.lib
                Winmm.lib
        )
    endif ()

    #message(CMAKE_VERSION="${CMAKE_VERSION}")
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.6.0")
        # Define WayView as startup project. https://stackoverflow.com/questions/7304625/how-do-i-change-the-startup-project-of-a-visual-studio-solution-via-cmake
        #message("CMAKE_VERSION is grater of equal 3.6")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WayView)
    endif()

    # define preprocessor macro contain the configuration name https://stackoverflow.com/questions/66571691/cmake-create-a-makefile-supports-debug-and-release-with-different-configuration
    target_compile_definitions(WayView PRIVATE MY_CONFIG_DEFINE=\"$<CONFIG>\")
elseif(UNIX)
    
endif()

set_target_properties(WayView
    PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)
