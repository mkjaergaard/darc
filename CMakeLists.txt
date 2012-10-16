cmake_minimum_required(VERSION 2.8)
project(darc_ns)

find_package(catkin)

find_package(Boost COMPONENTS system thread regex signals serialization)
find_package(catkin COMPONENTS beam darc_common)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES darc_ns)

include_directories(include)
include_directories(${catkin_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_library(darc_ns
  src/ns_service.cpp
  src/entry.cpp
  src/local_tag.cpp
  src/local_ns.cpp)

set(my_LIBRARIES
  darc_ns
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  -lstdc++)

# Add test cpp file
catkin_add_gtest(darc_ns_gtest_name_server test/name_server_gtest.cpp)
target_link_libraries(darc_ns_gtest_name_server ${my_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
