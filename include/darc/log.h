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

namespace darc
{
class Log
{
public:
  typedef enum {
    LOG_ALL = 0,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
  } LevelType;

private:
  static LevelType current_level_;
  static const char* level_names_[];

public:
  static LevelType& level();
  static void report(LevelType level, const char * msg);

  static void report2(LevelType level, const char * msg, ...);

};

}

#define DARC_AUTOTRACE() darc::Log::report(darc::Log::LOG_TRACE, __PRETTY_FUNCTION__)
#define DARC_INFO(...) darc::Log::report2(darc::Log::LOG_INFO, __VA_ARGS__)
#define DARC_WARNING(arg) darc::Log::report(darc::Log::LOG_WARNING, arg)
#define DARC_ERROR(arg) darc::Log::report(darc::Log::LOG_ERROR, arg)
#define DARC_FATAL(arg) darc::Log::report(darc::Log::LOG_FATAL, arg); exit(1)
