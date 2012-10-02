cmake_minimum_required(VERSION 2.8)
project(darc_ns)

find_package(catkin)
catkin_stack()

find_package(Boost COMPONENTS system thread regex signals serialization)
find_package(catkin COMPONENTS beam darc_common)

catkin_project(darc_ns
  INCLUDE_DIRS include)

include_directories(include)
include_directories(${catkin_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

set(my_LIBRARIES
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  -lstdc++)

# Add test cpp file
catkin_add_gtest(darc_ns_gtest_name_server test/name_server_gtest.cpp)
target_link_libraries(darc_ns_gtest_name_server ${my_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
