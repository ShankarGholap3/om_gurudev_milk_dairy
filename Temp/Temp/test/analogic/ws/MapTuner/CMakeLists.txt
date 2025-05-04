#-------------------------------------------------------------------------------
# CMake Project MapTuner Reader(server)  and Writer (client) components.
# Requires CMake 3.5 of higher!
# Written by Andre R. Oughton Feb 6, 2017
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5)


project( MapTunerReaderWriter )



set(CMAKE_CXX_FLAGS "-fPIC")
add_definitions(-std=gnu++0x)

set(USER_HOME $ENV{HOME})

if(UNIX)
#===============================================================================
#DICOS requires __LINUX__ flag
add_definitions( -D__LINUX__ )
endif(UNIX)


#-------------------------------------------------------------------------------
# Find the Qt Packages
find_package(Qt5Core 5 CONFIG REQUIRED)
find_package(Qt5Widgets 5 CONFIG REQUIRED)

add_definitions(${QT_DEFINITIONS})


#-------------------------------------------------------------------------------
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#-------------------------------------------------------------------------------
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


#-------------------------------------------------------------------------------
# Add Include Directories
include_directories(
  ./
  ../common
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)


#-------------------------------------------------------------------------------
# Application Headers
set (APP_HDRS_READER
MainWindow_reader.h
TunerIPC.h
../common/ConsoleWidget.h
)

set (APP_HDRS_WRITER
MainWindow_writer.h
TunerIPC.h
../common/ConsoleWidget.h
)



#-------------------------------------------------------------------------------
# Application Sources
set (APP_SRC_PREADER
  main_reader.cpp
  MainWindow_reader.cpp
  TunerIPC.cpp
  ../common/ConsoleWidget.cpp
)


set (APP_SRC_WRITER
  main_writer.cpp
  MainWindow_writer.cpp
  TunerIPC.cpp
  ../common/ConsoleWidget.cpp
  )

#-------------------------------------------------------------------------------
# Application UI source
set(APP_UI_READER
 MainWindow_reader.ui
)

set(APP_UI_WRITER
 MainWindow_writer.ui
)



qt5_wrap_ui(MapReaderApp_FORMS_HEADER ${APP_UI_READER})

qt5_wrap_ui(MapWriterApp_FORMS_HEADER ${APP_UI_WRITER})


#-------------------------------------------------------------------------------
# Tell CMake to create the QtDICOSTestApp executable
add_executable(MapReaderApp
  ${APP_SRC_PREADER}
  ${APP_HDRS_READER}
  ${APP_UI_READER}
)


add_executable(MapWriterApp
  ${APP_SRC_WRITER}
  ${APP_HDRS_WRITER}
  ${APP_UI_WRITER}
)


#-------------------------------------------------------------------------------
# Use the Widgets module from Qt 5.
target_link_libraries(MapReaderApp  
  Qt5::Widgets
  Qt5::Core
  -lpthread
)


target_link_libraries(MapWriterApp
  Qt5::Widgets
  Qt5::Core
  -lpthread
)


#-------------------------------------------------------------------------------
