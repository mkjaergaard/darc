/*
 * Copyright (c) 2011, Prevas A/S
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
 * DARC Node class
 *
 * \author Morten Kjaergaard
 */

// the old-style include guards are old.  #pragma once is supported on
// all major compilers
#pragma once

#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <darc/publish/manager.h>
#include <darc/procedure/manager.h>

namespace darc
{

  class Node
  {
  public:
    typedef boost::shared_ptr<Node> Ptr;

    virtual void run() = 0;
    virtual publish::Manager& getPublisherManager() = 0;
    virtual procedure::Manager& getProcedureManager() = 0;
    virtual void accept( const std::string& url ) = 0;
    virtual void connect( uint32_t remote_node_id, const std::string& url ) = 0;
    virtual void setNodeID( uint32_t node_id ) = 0;

    static Node::Ptr create();
  };

  // These should go *outside* the class, so that they are
  // forward-declarable (helps reduce compile time and coupling).
  // where possible, use the forward-declared versions in header
  // files, the full definitions of the classes in .cpp files.
  typedef boost::shared_ptr<Node> NodePtr;
}
