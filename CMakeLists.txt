cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(thsat)

find_package(Qt5Network 5.3 REQUIRED)
find_package(Qt5SerialPort 5.3 REQUIRED)

set(CMAKE_AUTOMOC ON)

add_subdirectory(src)
