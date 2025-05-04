#-------------------------------------------------------------------------------
# CMake Project for QtDICOS test Application
# Requires CMake 3.5 of higher!
# Written by Andre R. Oughton Feb 6, 2017
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5)


project( QtTDRTestApp )

set(CMAKE_CXX_FLAGS "-fPIC")
add_definitions(-std=gnu++0x)

set(USER_HOME $ENV{HOME})

if(UNIX)
#=============================================
#DICOS requires __LINUX__ flag
add_definitions( -D__LINUX__ )
endif(UNIX)

# VTK Required
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

#-----------------------------------------------------
# Find the Qt Packages
message("QT_USE_FILE: "  ${QT_USE_FILE})
find_package(Qt5Core 5.7.0 CONFIG REQUIRED)
find_package(Qt5Widgets 5.7.0 CONFIG REQUIRED)

add_definitions(${QT_DEFINITIONS})

#----------------------------------------------------------
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


if(UNIX)
    #DICOS requires __LINUX__ flag
    add_definitions( -D__LINUX__ )
set(USER_HOME $ENV{HOME})
set(BOOST_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0  )
set(ANALOGIC_ROOT ${USER_HOME}/$ENV{DEV_ROOT}/common/analogic )
set(BOOST_DIR     ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0/include )
set(VTK_INC_DIR   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/include/vtk-8.1/)
set(VTK_DIR       ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-8.1     CACHE PATH "VTK directory override" FORCE)
set(SDICOS_ROOT   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include/SDICOS)
set(SDICOS_INC    ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include)
set(DICOS_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/lib/SDICOS)
endif(UNIX)


set(VR_SRC_DIR    ../../../../analogic/ws/volume_renderer/)

message("SDICOS_ROOT:   " ${SDICOS_ROOT}  )
message("DICOS_LIBDIR:  " ${DICOS_LIBDIR} )
message("ANALOGIC_ROOT: " ${ANALOGIC_ROOT})


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


#--------------------------------------------------------
# Add Include Directories
include_directories(
  ./
  ../common
  ${BOOST_DIR}
  ${ANALOGIC_ROOT}/framework/inc/
  ${VR_SRC_DIR}
  ${SDICOS_ROOT}
  ${SDICOS_INC}
  ${VTK_INC_DIR}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)


#-----------------------------------------------------
# Application Headers
set (APP_HDRS
 MainWindow.h
 ../common/ConsoleWidget.h
 ../common/InputDialog3D.h
 ${ANALOGIC_ROOT}/framework/inc/Logger.h
 ${VR_SRC_DIR}/ThreatVolume.h
 ${VR_SRC_DIR}/ThreatObject.h
 ${VR_SRC_DIR}/ThreatList.h
 ${VR_SRC_DIR}/VRUtils.h
 ${VR_SRC_DIR}/ThreatDefs.h
 ${VR_SRC_DIR}/CenterSelectBoxWidget.h
 ${VR_SRC_DIR}/VolumeRendererBase.h
)


#-----------------------------------------------------
# Application Sources
set (APP_SRC
  main.cpp
  MainWindow.cpp
  ../common/ConsoleWidget.cpp
  ../common/InputDialog3D.cpp
  ${ANALOGIC_ROOT}/framework/src/Logger.cpp 
  ${VR_SRC_DIR}/ThreatVolume.cpp
  ${VR_SRC_DIR}/ThreatObject.cpp
  ${VR_SRC_DIR}/ThreatList.cpp
  ${VR_SRC_DIR}/VRUtils.cpp
  ${VR_SRC_DIR}/CenterSelectBoxWidget.cpp
  ${VR_SRC_DIR}/VolumeRendererBase.cpp
)

#-----------------------------------------------------
# Application Qt Resources
set(APP_QRC
 QtTDRTestApp.qrc
)

#-----------------------------------------------------
# Application UI source
set(APP_UI
 MainWindow.ui
)

qt5_wrap_ui(QtTDRTestApp_FORMS_HEADER ${APP_UI})

#-----------------------------------------------------
# Tell CMake to create the QtDICOSTestApp executable
add_executable(QtTDRTestApp
  ${APP_SRC}
  ${APP_HDRS}
  ${APP_UI}
  ${APP_QRC}
)



#-----------------------------------------------------
# Use the Widgets module from Qt 5.
 target_link_libraries(QtTDRTestApp
  ${VTK_LIBRARIES}
  ${BOOST_LIBDIR}/lib/libboost_log.a
  ${BOOST_LIBDIR}/lib/libboost_log_setup.a
  ${BOOST_LIBDIR}/lib/libboost_thread.a
  ${BOOST_LIBDIR}/lib/libboost_program_options.a
  ${BOOST_LIBDIR}/lib/libboost_filesystem.a
  ${BOOST_LIBDIR}/lib/libboost_iostreams.a
  ${BOOST_LIBDIR}/lib/libboost_system.a
  ${DICOS_LIBDIR}/libDICOSStatic.a
  Qt5::Widgets
  Qt5::Core
  -lpthread
)

#-----------------------------------------------------------------------------
