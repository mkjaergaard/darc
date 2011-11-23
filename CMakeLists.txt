cmake_minimum_required(VERSION 2.8)
project(darc)

if(FILT)
  set(CMAKE_CXX_COMPILER "/u/mkjargaard/usr/bin/g++")
endif()

find_package(catkin)
find_package(Boost COMPONENTS system thread)
find_package(ROS 12.04 COMPONENTS cpp_common rostime roscpp_traits roscpp_serialization sensor_msgs)


include_directories(include)

set(CMAKE_CXX_FLAGS "-Wall")

add_executable( node1 src/component1.cpp src/component2 src/main.cpp src/node.cpp)

target_link_libraries(node1 "-lstdc++" ${Boost_LIBRARIES} ${ROS_LIBRARIES})