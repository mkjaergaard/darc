cmake_minimum_required(VERSION 2.8)
project(roscpp_asio)

if(FILT)
  set(CMAKE_CXX_COMPILER "/u/mkjargaard/usr/bin/g++")
endif()

find_package(Boost COMPONENTS system thread)

include_directories(include)

add_executable( node1 src/component1.cpp src/component2 src/main.cpp src/node.cpp)

target_link_libraries(node1 "-lstdc++" ${Boost_LIBRARIES})