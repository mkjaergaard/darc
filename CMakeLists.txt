cmake_minimum_required(VERSION 2.8)
project(asmsg)

find_package(catkin)
catkin_stack()

catkin_project(asmsg
  INCLUDE_DIRS include
#  LIBRARIES asmsg
)

SET(CMAKE_BUILD_TYPE Debug)

find_package(Boost COMPONENTS system thread regex signals)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(asmsg_test
  src/main.cpp
)

target_link_libraries(asmsg_test ${Boost_LIBRARIES})
