#pragma once

#include <signal.h>
#include <boost/signals.hpp>

namespace darc
{

class system_signals
{
protected:
  typedef void(callback_type)(void);
  typedef boost::signal<callback_type> signal_type;

  signal_type sig_int_signal_;
  signal_type sig_term_signal_;

  static system_signals instance_;

  static void sig_int_handler(int)
  {
    instance_.sig_int_signal_();
  }

  static void sig_term_handler(int)
  {
    instance_.sig_term_signal_();
  }

public:
  system_signals()
  {
    signal(SIGINT, &system_signals::sig_int_handler);
    signal(SIGTERM, &system_signals::sig_term_handler);
  }

  static signal_type& sig_int_signal()
  {
    return instance_.sig_int_signal_;
  }

  static signal_type& sig_term_signal()
  {
    return instance_.sig_term_signal_;
  }

};

}
