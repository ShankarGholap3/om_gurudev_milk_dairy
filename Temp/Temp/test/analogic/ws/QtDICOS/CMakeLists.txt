#-------------------------------------------------------------------------------
# CMake Project for QtDICOS test Application
# Requires CMake 3.5 of higher!
# Written by Andre R. Oughton May 15, 2017
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5)

if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS "-fPIC")
    add_definitions(-std=gnu++0x)
endif()

project(QtDICOSTestApp)


set(USER_HOME $ENV{HOME})

if(UNIX)
#=============================================
#DICOS requires __LINUX__ flag
add_definitions( -D__LINUX__ )
endif(UNIX)

# VTK Required
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})
set(VTK_INC_DIR   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/include/vtk-8.1/)
set(VTK_DIR       ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-8.1     CACHE PATH "VTK directory override" FORCE)
set(SDICOS_ROOT   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include/SDICOS)
set(BOOST_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0  )
set(ANALOGIC_ROOT ${USER_HOME}/$ENV{DEV_ROOT}/common/analogic )
set(BOOST_DIR     ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0/include )
set(SDICOS_INC    ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include)
set(SDICOS_ROOT   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include/ )
set(DICOS_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/lib/SDICOS )
set(VOLREN_DIR    ${USER_HOME}/$ENV{DEV_ROOT}/workstation/src/analogic/ws/volume_renderer )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


#--------------------------------------------------------
# Add Include Directories
include_directories(
  ./
  ../common
  ${ANALOGIC_ROOT}/framework/inc
  ${BOOST_DIR}
  ${SDICOS_ROOT}
  ${SDICOS_INC}
  ${VOLREN_DIR}
)


# Find the QtWidgets library
find_package(Qt5Widgets)

#-----------------------------------------------------
# Application Headers
set (APP_HDRS
 ${ANALOGIC_ROOT}/framework/inc/Logger.h 
 MainWindow.h
 AboutDialog.h
 ../common/ConsoleWidget.h
 ReadDICOSHeader.h
 ${VOLREN_DIR}/VRUtils.h
 ${VOLREN_DIR}/ThreatObject.h
 ${VOLREN_DIR}/ThreatList.h
 ${VOLREN_DIR}/ThreatVolume.h
 ${VOLREN_DIR}/VolumeRendererBase.h
 ${VOLREN_DIR}/CenterSelectBoxWidget.h
)


#-----------------------------------------------------
# Application Sources
set (APP_SRC 
 main.cpp
 MainWindow.cpp  
 AboutDialog.cpp
 ../common/ConsoleWidget.cpp
 ReadDICOSHeader.cpp
 ${ANALOGIC_ROOT}/framework/src/Logger.cpp 
 ${VOLREN_DIR}/VRUtils.cpp
 ${VOLREN_DIR}/ThreatObject.cpp
 ${VOLREN_DIR}/ThreatList.cpp
 ${VOLREN_DIR}/ThreatVolume.cpp
 ${VOLREN_DIR}/VolumeRendererBase.cpp
 ${VOLREN_DIR}/CenterSelectBoxWidget.cpp
)


#-----------------------------------------------------
# Application UI source
set(APP_UI
 MainWindow.ui
)

#-----------------------------------------------------
set(APP_QRC
 QtDICOSTestApp.qrc
)


#-----------------------------------------------------
# Make the UI Headers
qt5_wrap_ui(Cmake_form_hdr ${APP_UI})


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


#-----------------------------------------------------
# Tell CMake to create the QtDICOSTestApp executable
add_executable(QtDICOSTestApp
  ${APP_SRC}
  ${APP_HDRS}
  ${APP_UI}
  ${APP_QRC}
)



#-----------------------------------------------------
# Use the Widgets module from Qt 5.
target_link_libraries( QtDICOSTestApp
    ${VTK_LIBRARIES}
    Qt5::Widgets
    ${BOOST_LIBDIR}/lib/libboost_log.a
    ${BOOST_LIBDIR}/lib/libboost_log_setup.a
    ${BOOST_LIBDIR}/lib/libboost_thread.a
    ${BOOST_LIBDIR}/lib/libboost_program_options.a
    ${BOOST_LIBDIR}/lib/libboost_filesystem.a
    ${BOOST_LIBDIR}/lib/libboost_iostreams.a
    ${BOOST_LIBDIR}/lib/libboost_system.a
    ${DICOS_LIBDIR}/libDICOSStatic.a
    -lpthread
    )


  #-----------------------------------------------------------------------------
