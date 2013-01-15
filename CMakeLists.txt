cmake_minimum_required(VERSION 2.8)
project(darc)

find_package(catkin)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES darc
  DEPENDS cpp_common roscpp_serialization roscpp_traits rostime)

find_package(Boost COMPONENTS system thread regex signals)
find_package(catkin REQUIRED COMPONENTS cpp_common rostime roscpp_traits roscpp_serialization)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS})

add_library(darc SHARED
  src/lib/component.cpp
  src/lib/primitive.cpp
  src/lib/node.cpp
  src/lib/registry.cpp
  src/lib/owner.cpp
  src/lib/id.cpp
  src/lib/log.cpp
  src/lib/tag_manager.cpp
  src/lib/timer/deadline_timer.cpp
  src/lib/pubsub/manager.cpp
  src/lib/pubsub/remote_dispatcher.cpp
  src/lib/pubsub/state_interface.cpp
  src/lib/procedure/manager.cpp
  src/lib/procedure/remote_dispatcher.cpp
  src/lib/procedure/system_state_interface.cpp
  src/lib/network/udp/protocol_manager.cpp
  src/lib/network/zmq/protocol_manager.cpp
)

install(DIRECTORY include/
  DESTINATION include
        FILES_MATCHING PATTERN "*.h")

install(TARGETS darc
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

target_link_libraries(darc ${Boost_LIBRARIES} ${ROS_LIBRARIES} -lzmq)

add_subdirectory(src/pybindings)
