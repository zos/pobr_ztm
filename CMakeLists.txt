cmake_minimum_required(VERSION 2.8)
project( POBR )

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")
add_definitions(-std=c++0x)

find_package( OpenCV REQUIRED )
add_executable( POBR ztm.cpp Common.cpp Momentum.cpp Operations.cpp )
target_link_libraries( POBR ${OpenCV_LIBS} )
