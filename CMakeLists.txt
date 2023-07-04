cmake_minimum_required(VERSION 2.8)
project(DisplayImage)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} linux_src)

#serial library code from https://github.com/Christian-Prather/youtube-serail-tutorial/blob/main/CMakeLists.txt
include_directories(serial serial/include serial/src)
set(serial_SRCS serial/src/serial.cc serial/include/serial/serial.h serial/include/serial/v8stdint.h serial/src/impl/list_ports/list_ports_linux.cc)
list(APPEND serial_SRCS serial/src/impl/unix.cc)
add_library( serial ${serial_SRCS})
target_link_libraries(serial rt)

add_executable(main linux_src/main.cpp linux_src/FileWriter.cpp linux_src/ImageProcessing.cpp linux_src/Controller.cpp linux_src/UARTInterface.cpp linux_src/DemoProgram.cpp linux_src/DataPuffer.cpp linux_src/Filter.cpp)
target_link_libraries(main ${OpenCV_LIBS} pthread serial)

add_executable(output-cleaner linux_src/output-cleaner.cpp)

add_executable(calibrate-cameras linux_src/calibrate-cameras.cpp linux_src/ImageProcessing.cpp)
target_link_libraries(calibrate-cameras ${OpenCV_LIBS})

add_executable(reset-board linux_src/reset-board.cpp linux_src/UARTInterface.cpp)
target_link_libraries(reset-board serial)