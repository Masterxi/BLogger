cmake_minimum_required(VERSION 3.6)
project (BLoggerExample)
include_directories("include")
find_package(Threads)
add_executable(BLoggerExample Example/Example.cpp)
target_link_libraries (BLoggerExample ${CMAKE_THREAD_LIBS_INIT})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BLoggerExample)