cmake_minimum_required(VERSION 2.8)
project(asmsg)

find_package(catkin)
catkin_stack()

catkin_project(asmsg
  INCLUDE_DIRS include
#  LIBRARIES asmsg
)

find_package(Boost COMPONENTS system signals thread)
find_package(catkin COMPONENTS darc_common)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS})

catkin_add_gtest(asmsg_gtest test/asmsg_gtest.cpp)
target_link_libraries(asmsg_gtest
  ${GTEST_BOTH_LIBRARIES}
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  -pthread)
