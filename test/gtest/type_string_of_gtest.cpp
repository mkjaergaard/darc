#include <gtest/gtest.h>

#include <iris/glog.hpp>

#include <std_msgs/String.h>
#include <darc/type_string_of.hpp>

struct blah
{
};

TEST(TypeStringOfTest, Test1)
{
  std::cout << darc::type_string_of<int*>::name() << std::endl;
  std::cout << darc::type_string_of<std::string*>::name() << std::endl;
  std::cout << darc::type_string_of<std_msgs::String>::name() << std::endl;

  std::cout << darc::is_ros<int>::value << std::endl;
  std::cout << darc::is_ros<std_msgs::String>::value << std::endl;
  std::cout << darc::is_ros<blah>::value << std::endl;

  std::cout << sizeof(char[1]) << ":" << sizeof(char[2]) << std::endl;


}
