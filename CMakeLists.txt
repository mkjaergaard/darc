cmake_minimum_required(VERSION 2.8)
project(darc)

find_package(catkin)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES darc
  CATKIN_DEPENDS zeromq_catkin
)

#find_package(catkin REQUIRED COMPONENTS roscpp_serialization zeromq_catkin)
find_package(catkin REQUIRED COMPONENTS zeromq_catkin)
find_package(Boost REQUIRED COMPONENTS system thread regex signals serialization)
find_package(iris REQUIRED)
find_package(Threads REQUIRED)

include_directories(include)
include_directories(${catkin_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${iris_INCLUDE_DIRS})
message(STATUS "ZMQ DIRS" "${zeromq_catkin_INCLUDE_DIRS}")
message(STATUS "catkin DIRS" "${catkin_INCLUDE_DIRS}")

# lib
add_library(darc
  # common
  src/lib/id.cpp
  src/lib/id_arg.cpp
  # peer
  src/lib/peer/peer.cpp
  src/lib/peer/system_signals.cpp
  # network
  src/lib/network/network_manager.cpp
  src/lib/network/inbound_link_base.cpp
  src/lib/network/zmq/zmq_protocol_manager.cpp
  src/lib/network/zmq/zmq_worker.cpp
  src/lib/network/zmq/zmq_listen_worker.cpp
  src/lib/network/zmq/zmq_connect_worker.cpp
  # ns
  src/lib/ns/ns_service.cpp
  src/lib/ns/local_tag.cpp
  # pubsub
  src/lib/primitives/topics/message_service.cpp
)
target_link_libraries(darc ${Boost_LIBRARIES} ${iris_LIBRARIES} ${catkin_LIBRARIES} -lstdc++)

# Tests
add_subdirectory(test)

# Install
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hpp")
