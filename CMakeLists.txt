cmake_minimum_required(VERSION 3.16)
project(Tusage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "C:/Qt/6.6.3/mingw_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Tusage main.cpp)
target_link_libraries(Tusage Qt6::Widgets)