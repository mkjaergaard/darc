cmake_minimum_required(VERSION 2.8)

# Manual Test Executables
add_executable(darc_test_shutdown1 manual/shutdown_test.cpp)
target_link_libraries(darc_test_shutdown1 darc)

# GTest
catkin_add_gtest(darc_gtest_type_string_of gtest/type_string_of_gtest.cpp)
target_link_libraries(darc_gtest_type_string_of darc ${GTEST_BOTH_LIBRARIES})

catkin_add_gtest(darc_gtest_buffer gtest/buffer_gtest.cpp)
target_link_libraries(darc_gtest_buffer darc ${GTEST_BOTH_LIBRARIES})

catkin_add_gtest(darc_gtest_network gtest/network_test.cpp)
target_link_libraries(darc_gtest_network darc ${GTEST_BOTH_LIBRARIES})
