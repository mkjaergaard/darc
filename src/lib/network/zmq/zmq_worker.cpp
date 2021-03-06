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

#include <darc/network/zmq/zmq_worker.hpp>

namespace darc
{
namespace network
{
namespace zeromq
{

zmq_worker::zmq_worker(zmq_protocol_manager * parent,
                       zmq::context_t& context,
                       int socket_type) :
  parent_(parent),
  socket_(context, socket_type)
{
}

void zmq_worker::work()
{
  try
  {
    while(1)
    {
      slog<iris::Debug>("ZeroMQ Waiting");
      work_receive();
    }
  }
  catch(zmq::error_t& e)
  {
    if(e.num() == ETERM)
    {
      // Expect an ETERM (Happens when we are shutting down)
    }
    else if(e.num() == EINTR)
    {
      slog<iris::Warning>("ZeroMQ EINT exception");
    }
    else
    {
      throw e; // TODO: Handle possible errors properly!
    }
  }
  slog<iris::Info>("ZeroMQ Exiting work thread");
  socket_.close();
}

}
}
}
