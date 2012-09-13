cmake_minimum_required(VERSION 2.8)
project(darc_common)

find_package(catkin)
catkin_stack()

find_package(Boost COMPONENTS system thread regex signals serialization)
find_package(llog)

include_directories(/usr/include/c++/4.6)
include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${llog_INCLUDE_DIRS})

add_library(darc_common
  src/id.cpp
  src/container_manager.cpp
  src/container_base.cpp
)

target_link_libraries(darc_common ${Boost_LIBRARIES} ${llog_LIBRARIES} -lstdc++)

# Add test cpp file
catkin_add_gtest(darc_common_gtest_buffer test/buffer_gtest.cpp)
target_link_libraries(darc_common_gtest_buffer darc_common ${GTEST_BOTH_LIBRARIES})

# Add test cpp file
add_gtest(darc_common_gtest_distributed test/distributed_gtest.cpp)
target_link_libraries(darc_common_gtest_distributed darc_common ${GTEST_BOTH_LIBRARIES})
