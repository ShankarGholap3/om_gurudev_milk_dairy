cmake_minimum_required(VERSION 2.8.12)
 
PROJECT(GaussianBench)
 
find_package(VTK REQUIRED)
find_package(Qt5Core)

include(${VTK_USE_FILE} ${VTK_USE_QT})

set(APP_SRC
    GaussianBench.cpp
    vtkImageAlgorithmFilter.cpp
)

set(APP_HDR
   vtkImageAlgorithmFilter.h
)
 
add_executable(GaussianBench MACOSX_BUNDLE ${APP_SRC})
 
target_link_libraries(GaussianBench  ${VTK_LIBRARIES})


