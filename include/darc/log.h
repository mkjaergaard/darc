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
 * DARC Log Stuff
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <iostream>
#include <fstream>

namespace darc
{
class Log
{
public:
  typedef enum {
    LOG_ALL = 0,
    LOG_DEBUG,
    LOG_TRACE,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
  } LevelType;

private:
  static LevelType current_level_;
  static const char* level_names_[];
  static std::ofstream log_file_;
  static bool log_to_file_;

public:
  static LevelType getLevel();
  static void setLevel(LevelType);
  static void logToFile(const std::string& filename);
  static void logToConsole();
  static void report(LevelType level, const char * msg, ...);

};

}

#define DARC_AUTOTRACE() darc::Log::report(darc::Log::LOG_TRACE, __PRETTY_FUNCTION__)
#define DARC_DEBUG(...) darc::Log::report(darc::Log::LOG_DEBUG, __VA_ARGS__)
#define DARC_TRACE(...) darc::Log::report(darc::Log::LOG_TRACE, __VA_ARGS__)
#define DARC_INFO(...) darc::Log::report(darc::Log::LOG_INFO, __VA_ARGS__)
#define DARC_WARNING(...) darc::Log::report(darc::Log::LOG_WARNING, __VA_ARGS__)
#define DARC_ERROR(...) darc::Log::report(darc::Log::LOG_ERROR, __VA_ARGS__)
#define DARC_FATAL(...) darc::Log::report(darc::Log::LOG_FATAL, __VA_ARGS__); exit(1)
