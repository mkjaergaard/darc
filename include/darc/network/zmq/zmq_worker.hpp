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

#include <zmq.hpp>
#include <boost/thread.hpp>
#include <iris/static_scope.hpp>

namespace darc
{
namespace network
{
namespace zeromq
{

class zmq_protocol_manager;

class zmq_worker : public iris::static_scope<iris::Info>
{
protected:
  zmq_protocol_manager * parent_;
  zmq::socket_t socket_;
  boost::thread thread_;

public:
  zmq_worker(zmq_protocol_manager * parent,
             zmq::context_t& context,
             int socket_type);

  virtual ~zmq_worker()
  {
    socket_.close();
    thread_.join();
  }

  zmq::socket_t& socket()
  {
    return socket_;
  }

protected:
  void run()
  {
    thread_ = boost::thread(boost::bind(&zmq_worker::work, this));
  }

  bool has_more()
  {
    int64_t more;
    size_t more_size = sizeof(more);
    socket_.getsockopt (ZMQ_RCVMORE, &more, &more_size);
    return (more != 0);
  }
/*
  void check_ok(bool condition)
  {
    assert(condition);
  }
*/
protected:
  virtual void work_receive() = 0;
  void work();

};

} // namespace zeromq
} // namespace network
} // namespace darc
