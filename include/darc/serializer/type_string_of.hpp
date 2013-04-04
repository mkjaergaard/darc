#pragma once

////////////
// Check for member
// http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence/264088#264088
// http://stackoverflow.com/questions/1005476/how-to-detect-whether-there-is-a-specific-member-variable-in-class

namespace darc
{

template<typename T>
struct type_string_of
{
  static const char* name()
  {
    return name_impl(ros::message_traits::IsMessage<T>());
  }

  static const char* name_impl(ros::message_traits::TrueType)
  {
    return ros::message_traits::DataType<T>::value();
  }

  static const char* name_impl(ros::message_traits::FalseType)
  {
    return typeid(T).name();
  }
};

// SFINAE test
template <typename T>
class is_ros
{
  template<typename C>
  static char my_type(typeof(ros::message_traits::DataType<C>::value));

  template<typename C>
  static long my_type(...);

public:
    enum { value2 = sizeof(my_type<T>("")) == sizeof(char) };
    static const bool value = ros::message_traits::IsMessage<T>::value;
};

}


