cmake_minimum_required(VERSION 2.8)
project(darc_common)

find_package(catkin)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES darc_common
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

add_library(darc_common
  src/id.cpp
  src/id_arg.cpp
  src/container_manager.cpp
  src/container_base.cpp
  src/peer.cpp
  src/system_signals.cpp
  src/network/network_manager.cpp
  src/network/inbound_link_base.cpp
  src/network/zmq/zmq_protocol_manager.cpp
  src/network/zmq/zmq_worker.cpp
  src/network/zmq/zmq_listen_worker.cpp
  src/network/zmq/zmq_connect_worker.cpp
)

target_link_libraries(darc_common ${Boost_LIBRARIES} ${iris_LIBRARIES} ${catkin_LIBRARIES} -lstdc++)

add_executable(darc_common_test_shutdown1 test/shutdown_test.cpp)
target_link_libraries(darc_common_test_shutdown1 darc_common )

# Add test cpp file
catkin_add_gtest(darc_common_gtest_type_string test/type_string_of_gtest.cpp)
target_link_libraries(darc_common_gtest_type_string darc_common ${GTEST_BOTH_LIBRARIES})

# Add test cpp file
catkin_add_gtest(darc_common_gtest_buffer test/buffer_gtest.cpp)
target_link_libraries(darc_common_gtest_buffer darc_common ${GTEST_BOTH_LIBRARIES})

# Add test cpp file
catkin_add_gtest(darc_common_gtest_distributed test/distributed_gtest.cpp)
target_link_libraries(darc_common_gtest_distributed darc_common ${GTEST_BOTH_LIBRARIES})

# Add test cpp file
catkin_add_gtest(darc_common_gtest_shared_map test/shared_map_gtest.cpp)
target_link_libraries(darc_common_gtest_shared_map darc_common ${GTEST_BOTH_LIBRARIES})

# Add test cpp file
catkin_add_gtest(darc_common_gtest_network test/network_test.cpp)
target_link_libraries(darc_common_gtest_network darc_common ${GTEST_BOTH_LIBRARIES})

# Install
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hpp")
