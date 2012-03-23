/*
 * Copyright (c) 2012, Prevas A/S
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
 * DARC Log Impl
 *
 * \author Morten Kjaergaard
 */

#include <darc/log.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <boost/date_time/posix_time/time_formatters.hpp>

namespace bt = boost::posix_time;

namespace darc
{

Log::LevelType Log::current_level_ = Log::LOG_TRACE;
bool Log::log_to_file_ = false;
std::ofstream Log::log_file_;

const char* Log::level_names_[] = {"?????","DEBUG","TRACE","INFO ","WARN ","ERROR", "FATAL"};

void Log::logToFile(const std::string& filename)
{
  log_file_.open(filename.c_str());
  log_to_file_ = true;
}

void Log::logToConsole()
{
  log_file_.close();
  log_to_file_ = false;
}

Log::LevelType Log::getLevel()
{
  return current_level_;
}

void Log::setLevel(Log::LevelType new_level)
{
  current_level_ = new_level;
}

void Log::report(Log::LevelType level, const char * msg, ...)
{
  if(level >= current_level_)
  {
    char buffer[512];
    sprintf(buffer, "[%s %s] %s", level_names_[level], bt::to_simple_string(bt::microsec_clock::universal_time().time_of_day()).c_str(), msg );

    char buffer2[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer2, buffer, args);
    va_end(args);

    if(log_to_file_)
    {
      log_file_ << buffer2 << std::endl;
      log_file_.flush();
    }
    else
    {
      std::cout << buffer2 << std::endl;
    }

  }
}



}
