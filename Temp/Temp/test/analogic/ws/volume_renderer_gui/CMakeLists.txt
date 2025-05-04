#-------------------------------------------------------------------------------
# CMake build file for Stand Alone Volume Renderer Application
# Copyright 2018 Analogic Corp.
# Andre R. Oughton
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

project(VolumeRenderer C CXX)

set(CMAKE_VERBOSE_MAKEFILE on)


#-------------------------------------------------------------------------------
#DICOS requires __LINUX__ flag
add_definitions( -D__LINUX__ )
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Test Flag for debugging VTK leaks.
# add_definitions( -DVTK_DEBUG_LEAKS)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
set(USER_HOME     $ENV{HOME})
set(BOOST_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0  )
set(WS_ROOT       ${USER_HOME}/$ENV{DEV_ROOT}/workstation/src)
set(ANALOGIC_ROOT ${USER_HOME}/$ENV{DEV_ROOT}/common/analogic)
set(SCANCOM_INC   ${USER_HOME}/$ENV{DEV_ROOT}/scanner/common/inc )
set(SCANCOM_SRC   ${USER_HOME}/$ENV{DEV_ROOT}/scanner/common/src )
set(RIAL_INC      ${USER_HOME}/$ENV{DEV_ROOT}/common/analogic/rial/inc )
set(BOOST_DIR     ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/alog-boost-1.60.0/include )
set(NSS_INCDIR    ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/nss_agent-04.04.00-2/include)
set(SDICOS_ROOT   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include/SDICOS)
set(SDICOS_INC    ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/include)
set(DICOS_LIBDIR  ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/dicos/lib/SDICOS)
set(Qt5Xml_INCDIR $ENV{QT_DIR}5.7/gcc_64/include/QtXmlPatterns/)
set(Qt5QML_INCDIR $ENV{QT_DIR}5.7/gcc_64/include/QtQml/)
set(QTLIB_DIR     $ENV{QT_DIR}5.7/gcc_64/lib/)
set(ULOC_LIBDIR   /usr/local/lib )
set(VOLREN_SRC_DIR ../../../../analogic/ws/volume_renderer)

#-------------------------------------------------------------------------------
# VTK 8.1 is the Current Version - June 25 - 2018
set(VTK_INC_DIR   ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/include/vtk-8.1/)
set(VTK_DIR       ${USER_HOME}/$ENV{DEV_ROOT}/workstation/3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-8.1     CACHE PATH "VTK directory override" FORCE)
#-------------------------------------------------------------------------------





add_definitions(-DWORKSTATION)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Uncomment to see output for debugging
# message("SDICOS INCLUDE:" ${SDICOS_ROOT}  )
# message("BOOST_LIBDIR:" ${BOOST_LIBDIR} )
# message("VTK INCLUDE DIR:" ${VTK_INC_DIR} )
# message("DEV ROOT:" $ENV{DEV_ROOT})
message("VTK_DIR: " ${VTK_DIR})
#-------------------------------------------------------------------------------

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# ARO-NOT: We no longer build for window but this is a palce holder for
#   showing how.
# if (WIN32)
# set(CMAKE_EXE_LINKER_FLAGS  "-Wl,--large-address-aware" )
# ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
# endif(WIN32)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#message("%%%%%%%%%%%%%%% QT_DIR="  $ENV{QT_DIR} )
# message("DEV_ROOT=" $ENV{DEV_ROOT})

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x -pg)
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#===============================================================================
# Turnining on gprof testing 
# set(CMAKE_CXX_FLAGD ${CMAKE_CXX_FLAGS} -pg)
set(CMAKE_EXE_LINKERFLAGS ${CMAKE_EXE_LINKER_FLAG} -pg)
set(CMAKE_SHARED_LINKER_FLAG ${CMAKE_SHARED_LINKER_FLAG} -pg)
#===============================================================================
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)





#===============================================================================
include_directories(
  ./
  ../common
  ${ANALOGIC_ROOT}/framework/inc
  ${BOOST_DIR}
  ${SDICOS_ROOT}
  ${SDICOS_INC}
  ${VTK_INC_DIR}
  ${VOLREN_SRC_DIR}
  ${WS_ROOT}
  ${SCANCOM_INC}
  ${RIAL_INC}
  ${Qt5Xml_INCDIR}
  ${Qt5QML_INCDIR}
  ${NSS_INCDIR}
)

#===============================================================================





qt5_wrap_ui(Cmake_form_hdr MainWindow.ui)

QT5_WRAP_UI(Cmake_form_hdr CropDialog.ui)

set(APP_QRC
  VolRender.qrc
)



set(APP_HDRS
 MainWindow.h
 DataManager.h
 ../common/ConsoleWidget.h
 ../common/SelectFromListDialog.h
 ../common/InputDialog3D.h
 CropDialog.h
 LogWindow.h
 BagQueueItem.h
 VRGUIUtils.h
 ${ANALOGIC_ROOT}/framework/inc/Logger.h 
 ${VOLREN_SRC_DIR}/ScannerDefs.h
 ${VOLREN_SRC_DIR}/VolumeRendererConfig.h
 ${VOLREN_SRC_DIR}/VolumeRendererInterface.h
 ${VOLREN_SRC_DIR}/VolumeRendererWidget.h
 ${VOLREN_SRC_DIR}/VolumeRendererBase.h
 ${VOLREN_SRC_DIR}/VolumeRenderer.h
 ${VOLREN_SRC_DIR}/OpenGLVolumeWidget.h
 ${VOLREN_SRC_DIR}/ViewLabelWidget.h
 ${VOLREN_SRC_DIR}/MarginWidget.h
 ${VOLREN_SRC_DIR}/AxisWidget.h
 ${VOLREN_SRC_DIR}/RulerWidget.h
 ${VOLREN_SRC_DIR}/ThreatObject.h
 ${VOLREN_SRC_DIR}/ThreatList.h
 ${VOLREN_SRC_DIR}/ThreatVolume.h
 ${VOLREN_SRC_DIR}/OpenGLTouchHandler.h
 ${VOLREN_SRC_DIR}/MemUtils.h
 ${VOLREN_SRC_DIR}/VRUtils.h
 ${VOLREN_SRC_DIR}/MouseInteractorStylePP.h
 ${VOLREN_SRC_DIR}/ThreatDefs.h
 ${VOLREN_SRC_DIR}/CenterSelectBoxWidget.h
 ${VOLREN_SRC_DIR}/RubberBandPickVolume.h 
 ${WS_ROOT}/analogic/ws/VREnums.h
)

message("ANALOGIC_ROOT=" ${ANALOGIC_ROOT})

set(APP_SRC
 main.cpp
 MainWindow.cpp 
 DataManager.cpp
 ../common/ConsoleWidget.cpp
 ../common/SelectFromListDialog.cpp
 ../common/InputDialog3D.cpp
 CropDialog.cpp
 LogWindow.cpp
 VRGUIUtils.cpp
 ${ANALOGIC_ROOT}/framework/src/Logger.cpp 
 ${VOLREN_SRC_DIR}/VolumeRendererConfig.cpp
 ${VOLREN_SRC_DIR}/VolumeRendererWidget.cpp
 ${VOLREN_SRC_DIR}/VolumeRendererBase.cpp
 ${VOLREN_SRC_DIR}/VolumeRenderer.cpp
 ${VOLREN_SRC_DIR}/OpenGLVolumeWidget.cpp
 ${VOLREN_SRC_DIR}/ViewLabelWidget.cpp
 ${VOLREN_SRC_DIR}/MarginWidget.cpp
 ${VOLREN_SRC_DIR}/AxisWidget.cpp
 ${VOLREN_SRC_DIR}/RulerWidget.cpp
 ${VOLREN_SRC_DIR}/ThreatObject.cpp
 ${VOLREN_SRC_DIR}/ThreatList.cpp
 ${VOLREN_SRC_DIR}/ThreatVolume.cpp
 ${VOLREN_SRC_DIR}/OpenGLTouchHandler.cpp
 ${VOLREN_SRC_DIR}/MemUtils.cpp
 ${VOLREN_SRC_DIR}/VRUtils.cpp 
 ${VOLREN_SRC_DIR}/CenterSelectBoxWidget.cpp
 ${VOLREN_SRC_DIR}/RubberBandPickVolume.cpp 
 )
 

set(APP_UI
 MainWindow.ui
 CropDialog.ui
)




#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# ARO-NOT: We no longer build for window but this is a palce holder for showing
# hoe to add resources with MINGW.
#
# Important for building CMake applications with Icons!
# This sets up the exe icon for windows under mingw.
#if(WIN32)
# Resource compilation for MinGW
#
#ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/VRProject_rc.o
#COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
#-i${CMAKE_CURRENT_SOURCE_DIR}/VRProject.rc
#-o ${CMAKE_CURRENT_BINARY_DIR}/VRProject_rc.o)
#
##SET(EXE_SRCS ${EXE_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/VRProject_rc.o)
#SET(EXE_SRCS ${EXE_SRCS} )
#endif(WIN32)
#
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#message("CMAke Include path=" ${CMAKE_INCLUDE_PATH})
#message("CMAke LIB path=" ${LIBRARY_PATH})


add_executable( VolumeRenderer
    ${APP_SRC}
    ${APP_HDRS}
    ${APP_UI}
    ${APP_QRC}
 )

target_link_libraries( VolumeRenderer
    ${VTK_LIBRARIES}
    ${BOOST_LIBDIR}/lib/libboost_log.a
    ${BOOST_LIBDIR}/lib/libboost_log_setup.a
    ${BOOST_LIBDIR}/lib/libboost_thread.a
    ${BOOST_LIBDIR}/lib/libboost_program_options.a
    ${BOOST_LIBDIR}/lib/libboost_filesystem.a
    ${BOOST_LIBDIR}/lib/libboost_iostreams.a
    ${BOOST_LIBDIR}/lib/libboost_system.a
    ${DICOS_LIBDIR}/libDICOSStatic.a
   # ${ULOC_LIBDIR}/libleaktracer.a
    ${QTLIB_DIR}/libQt5Xml.so
    ${QTLIB_DIR}/libQt5XmlPatterns.so
)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# End of CMakeLists.txt
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
