#1.指定cmake版本
cmake_minimum_required( VERSION 2.8 )
project( imgProject )
set( CMAKE_BUILD_TYPE "Debug" )
add_library(imgProjectShare SHARED img.c 
                                pixel.c)

add_executable(imgProject main.c)

target_link_libraries(imgProject imgProjectShare)
target_link_libraries(imgProject m)#链接math.h