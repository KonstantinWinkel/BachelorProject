cmake_minimum_required(VERSION 2.8)
project(DisplayImage)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} linux_src)

add_executable(FindAngles linux_src/FindAngles.cpp)
target_link_libraries(FindAngles ${OpenCV_LIBS} pthread)

add_executable(Main linux_src/main.cpp linux_src/FileWriter.cpp linux_src/ImageProcessing.cpp)
target_link_libraries(Main ${OpenCV_LIBS} pthread)