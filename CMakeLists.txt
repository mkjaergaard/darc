cmake_minimum_required(VERSION 2.8)
project(darc_pubsub)

find_package(catkin)

catkin_package(
  INCLUDE_DIRS include
)

find_package(Boost COMPONENTS system signals thread)
find_package(catkin COMPONENTS darc_common darc_ns beam)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS})

catkin_add_gtest(darc_pubsub_gtest test/darc_pubsub_gtest.cpp)
target_link_libraries(darc_pubsub_gtest
  ${GTEST_BOTH_LIBRARIES}
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  -pthread)
