/*
 * Copyright (c) 2013, Prevas A/S
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Prevas A/S nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \author Morten Kjaergaard
 */

#pragma once

#include <string>
#include <list>
#include <boost/bind.hpp>

namespace darc
{
namespace test
{

struct event_record
{
  std::string type;
  std::string data;
  std::string extra;

  static bool is_type(const event_record& record, const std::string& type)
  {
    return record.type == type;
  }
};

class event_list
{
protected:

  typedef std::list<event_record> list_type;
  list_type list_;

public:
  void event_callback(const std::string type, const std::string data, const std::string extra)
  {
    event_record r;
    list_type::iterator record = list_.insert(list_.begin(), r);
    record->type = type;
    record->data = data;
    record->extra = extra;
  }

  void clear()
  {
    list_.clear();
  }

  int count_type(const std::string& type)
  {
    int count = 0;
    for(list_type::iterator it = list_.begin();
        it != list_.end();
        it++)
    {
      if(it->type == type)
      {
        count++;
      }
    }
    return count;
  }

  void remove_type(const std::string& type)
  {
    list_.remove_if(boost::bind<bool>(&event_record::is_type, _1, type));
  }

  bool pop_type(const std::string& type)
  {
    for(list_type::iterator it = list_.begin();
        it != list_.end();
        it++)
    {
      if(it->type == type)
      {
        list_.erase(it);
        return true;
      }
    }
    return false;
  }

  bool is_empty()
  {
    return list_.empty();
  }

};

}
}
