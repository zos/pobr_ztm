cmake_minimum_required(VERSION 2.8)
project( POBR )

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")
add_definitions(-std=c++0x)

find_package( OpenCV REQUIRED )
add_executable( ztm ztm.cpp Common.cpp Momentum.cpp Operations.cpp Segmentation.cpp)
add_executable( test test.cpp Common.cpp Momentum.cpp Operations.cpp )
target_link_libraries( ztm ${OpenCV_LIBS} )
target_link_libraries( test ${OpenCV_LIBS} -lboost_unit_test_framework )
