cmake_minimum_required(VERSION 2.8)
project(darc)

set(CMAKE_PREFIX_PATH /opt/ros/fuerte)

#set(CMAKE_CXX_COMPILER clang)

find_package(catkin)
find_package(Boost COMPONENTS system thread)
find_package(ROS REQUIRED COMPONENTS cpp_common rostime roscpp_traits roscpp_serialization sensor_msgs std_msgs)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ROS_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -g")
#set(CMAKE_CXX_FLAGS "-Os")

add_library( darc SHARED src/node.cpp src/component_register.cpp)
target_link_libraries(darc ${Boost_LIBRARIES} ${ROS_LIBRARIES})

add_executable( node1 src/component1.cpp src/component2 src/node1.cpp)
target_link_libraries(node1 darc)

add_executable( node2 src/component1.cpp src/component2 src/node2.cpp)
target_link_libraries(node2 darc)
