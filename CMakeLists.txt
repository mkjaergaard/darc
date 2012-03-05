cmake_minimum_required(VERSION 2.8)
project(darc)

SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_PREFIX_PATH /opt/ros/fuerte)

find_package(catkin)
find_package(Boost COMPONENTS system thread regex signals)
find_package(ROS REQUIRED COMPONENTS cpp_common rostime roscpp_traits roscpp_serialization)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ROS_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -g")
#set(CMAKE_CXX_FLAGS "-Os")

add_library(darc SHARED
  src/lib/node.cpp
  src/lib/registry.cpp
  src/lib/owner.cpp
  src/lib/id.cpp
  src/lib/log.cpp
  src/lib/subcomponent_list.cpp
  src/lib/network/udp/protocol_manager.cpp
  src/lib/pubsub/manager.cpp
  src/lib/pubsub/event_listener.cpp
  src/lib/parameter/manager.cpp
  src/lib/parameter/parameter_abstract.cpp
  src/lib/parameter/server_component.cpp
)

target_link_libraries(darc ${Boost_LIBRARIES} ${ROS_LIBRARIES})
message(INFO {Boost_SYSTEM_LIBRARY_DEBUG})

catkin_project(darc
  VERSION 0.0.1
  INCLUDE_DIRS include
  LIBRARIES darc
  CFG_EXTRAS darc-macros.cmake)

#add_subdirectory(src/pybindings)

sphinx(${CMAKE_CURRENT_SOURCE_DIR}/doc ${CMAKE_CURRENT_BINARY_DIR}/doc)
