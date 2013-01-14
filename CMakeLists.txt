cmake_minimum_required(VERSION 2.8)
project(darc_common)

find_package(catkin)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES darc_common
)

find_package(Boost REQUIRED COMPONENTS system thread regex signals serialization)
find_package(iris REQUIRED)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${iris_INCLUDE_DIRS})

add_library(darc_common
  src/id.cpp
  src/id_arg.cpp
  src/container_manager.cpp
  src/container_base.cpp
  src/peer.cpp
  src/system_signals.cpp
  src/network/inbound_link_base.cpp
  src/network/zmq/protocol_manager.cpp
)

target_link_libraries(darc_common ${Boost_LIBRARIES} ${iris_LIBRARIES} -lstdc++ -lzmq)

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
target_link_libraries(darc_common_gtest_network darc_common ${GTEST_BOTH_LIBRARIES} -lzmq)

# Install
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hpp")
