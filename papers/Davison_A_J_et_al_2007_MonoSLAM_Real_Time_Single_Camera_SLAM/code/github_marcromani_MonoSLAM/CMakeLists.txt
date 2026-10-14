cmake_minimum_required(VERSION 3.16)

project(MonoSLAM_Davison CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(APPLE)
    foreach(prefix
            /opt/homebrew/opt/opencv@4
            /usr/local/opt/opencv@4
            /opt/homebrew/opt/opencv
            /usr/local/opt/opencv)
        if(EXISTS "${prefix}")
            list(APPEND CMAKE_PREFIX_PATH "${prefix}")
        endif()
    endforeach()
endif()

find_package(OpenCV 4 REQUIRED COMPONENTS
    core
    imgproc
    highgui
    calib3d
    videoio
)

add_library(monoslam_core
    MonoSLAM/camera.cpp
    MonoSLAM/feature.cpp
    MonoSLAM/map.cpp
    MonoSLAM/quaternion.cpp
    MonoSLAM/util.cpp
)

target_include_directories(monoslam_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/MonoSLAM
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(monoslam_core
    PUBLIC
        ${OpenCV_LIBS}
)

target_compile_options(monoslam_core
    PRIVATE
        -Wall
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-funroll-loops>
)

add_executable(monoslam
    MonoSLAM/main.cpp
)

target_link_libraries(monoslam
    PRIVATE
        monoslam_core
)

add_executable(calibration
    Calibration/calibration.cpp
)

target_include_directories(calibration
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(calibration
    PRIVATE
        ${OpenCV_LIBS}
)

target_compile_options(calibration
    PRIVATE
        -Wall
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-funroll-loops>
)
