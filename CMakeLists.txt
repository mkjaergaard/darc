cmake_minimum_required(VERSION 2.8)
project(darc)

set(CMAKE_PREFIX_PATH /tmp/wgbuild)

if(FILT)
  set(CMAKE_CXX_COMPILER "/u/mkjargaard/usr/bin/g++")
endif()

find_package(catkin)
find_package(Boost COMPONENTS system thread)
find_package(ROS 12.04 COMPONENTS cpp_common rostime roscpp_traits roscpp_serialization sensor_msgs std_msgs)


include_directories(include)
include_directories(${ROS_INCLUDE_DIRS})

#
set(CMAKE_CXX_FLAGS "-Wall -g")

add_executable( node1 src/component1.cpp src/component2 src/main.cpp src/component_register.cpp)
target_link_libraries(node1 "-lstdc++" ${Boost_LIBRARIES} ${ROS_LIBRARIES})

add_executable( node2 src/component1.cpp src/component2 src/main2.cpp src/component_register.cpp)
target_link_libraries(node2 "-lstdc++" ${Boost_LIBRARIES} ${ROS_LIBRARIES})