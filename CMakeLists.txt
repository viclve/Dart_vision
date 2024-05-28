cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(main)

file(GLOB camera_src Camera/*.cpp)
file(GLOB serial_src SerialCommunicator/*.cpp)

include_directories(
    Camera 
    Camera/MVCamera/include
    detector
    parameters
    # processor
    SerialCommunicator
    SerialCommunicator/serial/include
)

include_directories("/usr/include/eigen3")

find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

add_executable(main 
    main.cpp 
    Camera/camera.cpp
    parameters/params.cpp
    # processor/processor.cpp
    detector/detector.cpp
    ${camera_src}
    ${serial_src}
)

find_package(OpenCV REQUIRED)

message(STATUS "opencv version " ${OpenCV_VERSION})
message(STATUS "opencv directories " ${OpenCV_DIR})
include_directories(${OpenCV_INCLUDE_DIRS} .) 
target_link_libraries(main ${OpenCV_LIBS})  

# GLOG
find_package (glog REQUIRED)
target_link_libraries (main glog::glog)

target_link_libraries(main 
    ${PROJECT_SOURCE_DIR}/Camera/MVCamera/lib/x64/libMVSDK.so
    ${CERES_LIBRARIES}
    ${PROJECT_SOURCE_DIR}/SerialCommunicator/serial/lib/x64/libserial.a
)
