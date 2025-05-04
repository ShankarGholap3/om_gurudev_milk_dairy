# CMake build file for Flip Bag Application
# Copyright 2017 Analogic Corp.
# Andre R. Oughton
cmake_minimum_required(VERSION 2.8.11)

project(FlipBag C CXX)

#=====================================================================
# BOOST & Analog Root
# Build path variable definitions
if (WIN32)
set(USER_HOME $ENV{HOME})
STRING(REGEX REPLACE "\\\\" "/" USER_HOME "${USER_HOME}")
set(BOOST_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/boost_1_60_0-inst/lib)
set(ANALOGIC_ROOT ${USER_HOME}/$ENV{DEV_ROOT}/common/analogic)
set(BOOST_DIR     ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/boost_1_60_0-inst/include/boost-1_60)
endif(WIN32)


if(UNIX)
    #DICOS requires __LINUX__ flag
    add_definitions( -D__LINUX__ )
set(USER_HOME $ENV{HOME})
set(BOOST_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0  )
set(ANALOGIC_ROOT ${USER_HOME}/$ENV{DEV_ROOT}/common/analogic )
set(BOOST_DIR     ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0/include )
set(VTK_INC_DIR   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/include/vtk-6.3/)
set(VTK_DIR       ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-6.3     CACHE PATH "VTK directory override" FORCE)
set(SDICOS_ROOT   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include/SDICOS)
set(SDICOS_INC    ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include)
set(DICOS_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/lib/SDICOS)
endif(UNIX)

#--------------------------------------------
# Uncomment to see output for debugging
# message("SDICOS INCLUDE:" ${SDICOS_ROOT}  )
# message("BOOST_LIBDIR:" ${BOOST_LIBDIR} )
# message("VTK INCLUDE DIR:" ${VTK_INC_DIR} )
# message("DEV ROOT:" $ENV{DEV_ROOT})
message("VTK_DIR: " ${VTK_DIR})

set(VOLREN_SRC_DIR ../../../../analogic/ws/volume_renderer)

#=====================================================================

if (WIN32)
set(CMAKE_EXE_LINKER_FLAGS  "-Wl,--large-address-aware" )
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
endif(WIN32)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})
#===========================================================
include_directories(
  ./
  ../common
  ${ANALOGIC_ROOT}/framework/inc
  ${BOOST_DIR}
  ${SDICOS_ROOT}
  ${SDICOS_INC}
  ${VTK_INC_DIR}
  ${VOLREN_SRC_DIR}
)
#=============================================================

#--------------------------------------------
# Uncomment to see output for debugging
# message("XXX-BOOST DIR:" ${BOOST_DIR})


# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)

qt5_wrap_ui(Cmake_form_hdr MainWindow.ui)

#QT5_WRAP_UI(Cmake_form_hdr CropDialog.ui)

set(APP_QRC
  FlipBag.qrc
)



set (APP_HDRS
 MainWindow.h
 ../common/ConsoleWidget.h
 ${ANALOGIC_ROOT}/framework/inc/Logger.h 
 ${VOLREN_SRC_DIR}/VolumeRendererBase.h
 ${VOLREN_SRC_DIR}/VRUtils.h
 ${VOLREN_SRC_DIR}/ThreatVolume.h
 ${VOLREN_SRC_DIR}/ThreatObject.h
 ${VOLREN_SRC_DIR}/ThreatList.h

)



set (APP_SRC 
 main.cpp
 MainWindow.cpp 
 ../common/ConsoleWidget.cpp
 ${ANALOGIC_ROOT}/framework/src/Logger.cpp 
 ${VOLREN_SRC_DIR}/VolumeRendererBase.cpp
 ${VOLREN_SRC_DIR}/VRUtils.cpp
 ${VOLREN_SRC_DIR}/ThreatVolume.cpp
 ${VOLREN_SRC_DIR}/ThreatObject.cpp
 ${VOLREN_SRC_DIR}/ThreatList.cpp

 )
 

set(APP_UI
 MainWindow.ui
)


#message("CMAke Include path=" ${CMAKE_INCLUDE_PATH})
#message("CMAke LIB path=" ${LIBRARY_PATH})


# Tell CMake to create the VolumeRenderer executable
if(WIN32)
add_executable(FlipBag WIN32
  ${APP_SRC}
  ${APP_HDRS}
  ${APP_UI}
#  ${APP_QRC}
  #${CMAKE_CURRENT_BINARY_DIR}/VRProject_rc.o
  )

target_link_libraries( FlipBag
    ${VTK_LIBRARIES}
    ${BOOST_LIBDIR}/libboost_log-mgw49-mt-d-1_60.dll.a
    ${BOOST_LIBDIR}/libboost_log_setup-mgw49-mt-d-1_60.dll.a
    ${BOOST_LIBDIR}/libboost_thread-mgw49-mt-1_60.dll.a
    ${BOOST_LIBDIR}/libboost_program_options-mgw49-mt-1_60.dll.a
    ${BOOST_LIBDIR}/libboost_filesystem-mgw49-mt-d-1_60.dll.a
    ${BOOST_LIBDIR}/libboost_system-mgw49-mt-d-1_60.dll.a
    -lpthread    
    )
endif(WIN32)

if(UNIX)
  add_executable( FlipBag
    ${APP_SRC}
    ${APP_HDRS}
    ${APP_UI}
    ${APP_QRC}
    )

  target_link_libraries( FlipBag
    ${VTK_LIBRARIES}
    ${BOOST_LIBDIR}/lib/libboost_log.a
    ${BOOST_LIBDIR}/lib/libboost_log_setup.a
    ${BOOST_LIBDIR}/lib/libboost_thread.a
    ${BOOST_LIBDIR}/lib/libboost_program_options.a
    ${BOOST_LIBDIR}/lib/libboost_filesystem.a
    ${BOOST_LIBDIR}/lib/libboost_iostreams.a
    ${BOOST_LIBDIR}/lib/libboost_system.a
    ${DICOS_LIBDIR}/libDICOSStatic.a
    )
endif(UNIX)




