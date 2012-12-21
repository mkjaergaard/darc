#pragma once

#include <boost/function.hpp>

namespace darc
{
namespace test
{

template<typename T1, typename T2, typename T3>
class callback_monitor
{
public:
  int count;

  callback_monitor() :
    count(0)
  {
  }

  boost::function<void(const T1&, const T2&, const T3&)> functor()
  {
    return boost::bind(&callback_monitor::callback, this, _1, _2, _3);
  }

  void callback(const T1& v1, const T2& v2, const T3& v3)
  {
    count++;
  }

  int num_callbacks()
  {
    int v = count;
    count = 0;
    return v;
  }

  void reset()
  {
    count = 0;
  }

};

}
}
