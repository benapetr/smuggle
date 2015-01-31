# This is a build file for huggle (used with cmake)
# you will need to update it by hand
cmake_minimum_required (VERSION 2.8.9)
PROJECT(smuggle)
execute_process(COMMAND "${CMAKE_SOURCE_DIR}/prepare_cmake.sh" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
execute_process(COMMAND "${CMAKE_SOURCE_DIR}/update.sh" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
#  cmake <folder> -DQT5_BUILD=true
option(QT5_BUILD "Build smuggle using the Qt5 framework" FALSE)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTWEBKIT TRUE)
SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
if(WIN32)
    if(MINGW)
	SET(CMAKE_CXX_FLAGS "-mwindows -std=c++11")
    endif()
else()
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif()
#### Qt 4 and 5 ####
if(QT5_BUILD)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5WebKit REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Network REQUIRED)
    set(QT_INCLUDES
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5WebKit_INCLUDE_DIRS}
        ${Qt5Xml_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
    )
    include_directories(${QT_INCLUDES})
else()
    find_package(Qt4 REQUIRED)
endif()
if (PYTHON_BUILD)
    find_package(PythonLibs REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
endif()

file(GLOB srcx
    "*.cpp"
)
file (GLOB resx
    "*.qrc"
)
file (GLOB uixx
    "*.ui"
)

SET(smuggle_SOURCES ${srcx})
SET(smuggle_FORMS ${uixx})
SET(smuggle_RESOURCES ${resx})
if(QT5_BUILD)
    QT5_WRAP_CPP(smuggle_HEADERS_MOC ${smuggle_HEADERS})
    QT5_WRAP_UI(smuggle_FORMS_HEADERS ${smuggle_FORMS})
    QT5_ADD_RESOURCES(smuggle_RESOURCES_RCC ${smuggle_RESOURCES})
else()
    QT4_WRAP_CPP(smuggle_HEADERS_MOC ${smuggle_HEADERS})
    QT4_WRAP_UI(smuggle_FORMS_HEADERS ${smuggle_FORMS})
    QT4_ADD_RESOURCES(smuggle_RESOURCES_RCC ${smuggle_RESOURCES})
endif()
if (NOT QT5_BUILD)
  INCLUDE(${QT_USE_FILE})
endif()
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS( -DQT_USE_QSTRINGBUILDER )
ADD_EXECUTABLE(smuggle ${smuggle_SOURCES} ${smuggle_HEADERS_MOC} ${smuggle_FORMS_HEADERS} ${smuggle_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(smuggle ${QT_LIBRARIES})
if (WIN32)
    TARGET_LINK_LIBRARIES(smuggle dbghelp)
endif()
if (QT5_BUILD)
    qt5_use_modules(smuggle Core Gui Widgets WebKitWidgets WebKit Network Xml)
endif()
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/man/smuggle.1 DESTINATION share/man/man1)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/build/smuggle.desktop DESTINATION share/applications)
#####INSTALL(FILES ${CMAKE_SOURCE_DIR}/Resources/huggle3_newlogo.png DESTINATION share/huggle)
INSTALL(TARGETS smuggle DESTINATION bin)
INCLUDE(CPack)
