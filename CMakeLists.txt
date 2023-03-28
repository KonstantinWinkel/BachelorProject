cmake_minimum_required(VERSION 2.8)
project(DisplayImage)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} linux_src)

add_executable(main linux_src/main.cpp linux_src/FileWriter.cpp linux_src/ImageProcessing.cpp linux_src/Controller.cpp linux_src/UARTInterface.cpp)
target_link_libraries(main ${OpenCV_LIBS} pthread)