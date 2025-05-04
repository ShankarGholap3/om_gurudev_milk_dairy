#-------------------------------------------------------------------------------
# CMake build RectTube File 
# Copyright 2017 Analogic Corp.
# Andre R. Oughton
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.11)

project(RectTube C CXX)


if(UNIX)
add_definitions( -D__LINUX__ )
set(USER_HOME $ENV{HOME})
set(VTK_INC_DIR   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/include/vtk-6.3/)
set(VTK_DIR       ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-6.3     CACHE PATH "VTK directory override" FORCE)
endif(UNIX)

#--------------------------------------------
# Uncomment to see output for debugging
# message("VTK INCLUDE DIR:" ${VTK_INC_DIR} )
# message("DEV ROOT:" $ENV{DEV_ROOT})
message("VTK_DIR: " ${VTK_DIR})


add_definitions(-std=gnu++0x)
set(CMAKE_CXX_FLAGS "-fPIC")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

include_directories(
  ./  
  ${VTK_INC_DIR}
)
#=============================================================



# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)




#qt5_wrap_ui(Cmake_form_hdr MainWindow.ui)

set ( MOC_HEADERS
 MainWindow.h
)

set(APP_SRC 
 main.cpp 
 MainWindow.cpp
 RectTube.cpp    
 )
 
set (APP_HDRS
 RectTube.h
 RectTube.cpp 
)

set(APP_UI
 MainWindow.ui
)

set(APP_QRC
 RectTube.qrc
)



# using Qt meta-system (precompiler)
QT5_ADD_RESOURCES( RES_SOURCES ${APP_QRC} )
QT5_WRAP_UI( UI_HEADERS ${APP_UI} )
QT5_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )



# Tell CMake to create the RectTube executable
if(UNIX)
  add_executable( RectTube
    ${APP_SRC}
    ${APP_HDRS}
    ${APP_UI}
    ${APP_QRC}
    )

  target_link_libraries( RectTube
    ${VTK_LIBRARIES}   
    )
endif(UNIX)








