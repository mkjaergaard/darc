cmake_minimum_required(VERSION 2.8)
project(darc)

set(CMAKE_PREFIX_PATH /opt/ros/fuerte)

find_package(catkin)
find_package(Boost COMPONENTS system thread regex)
find_package(ROS REQUIRED COMPONENTS cpp_common rostime roscpp_traits roscpp_serialization)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ROS_INCLUDE_DIRS})

#set(CMAKE_CXX_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS "-Os")

#set(CMAKE_SHARED_LINKER_FLAGS "-pg")

add_library(darc SHARED
  src/lib/node.cpp
  src/lib/registry.cpp
  src/lib/pubsub/manager.cpp
  src/lib/owner.cpp
  src/lib/id.cpp
  src/lib/log.cpp
)

target_link_libraries(darc ${Boost_LIBRARIES} ${ROS_LIBRARIES} -ldl)

catkin_project(darc
  VERSION 0.0.1
  INCLUDE_DIRS include
  LIBRARIES darc
  CFG_EXTRAS darc-macros.cmake)

add_subdirectory(src/pybindings)
