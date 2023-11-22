cmake_minimum_required(VERSION 3.5)

project(sound5 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)



#set(OpenCV_DIR, "/Users/vladimir/Applications/open/build_opencv")
set(OpenCV_DIR, "//Users/vladimir/opencv/build/install/include/opencv4")

FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

set(PROJECT_SOURCES
        main.cpp
        soundanalyze.cpp
        soundanalyze.h
        soundanalyze.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(sound5
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        gnuplot-iostream.h mathematical_functions.h
        beamforming.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET sound5 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(sound5 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(sound5
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(sound5 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

#add_library(source STATIC ${src} )
#target_link_libraries("/Users/vladimir/opencv/build/install/include/opencv4")

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.sound5)
endif()
set_target_properties(sound5 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

#INCLUDEPATH += "/Users/vladimir/opencv/build/install/include/opencv4"
#LIBS += -L"/Users/vladimir/opencv/build/install/lib" \
#        -l"opencv_world"

include(GNUInstallDirs)
install(TARGETS sound5
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(sound5)
endif()
