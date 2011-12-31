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
 * DARC Node_Impl class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_NODE_IMPL_H_INCLUDED__
#define __DARC_NODE_IMPL_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <darc/node.h>
#include <darc/node_link_manager.h>
#include <darc/publish/remote_dispatcher_manager.h>
#include <darc/publish/local_dispatcher_manager.h>
#include <darc/procedure/local_dispatcher_manager.h>

namespace darc
{

class NodeImpl : public Node
{

private:
  boost::asio::io_service io_service_;

  //  boost::asio::signal_set signals_; not in boost 1.40

  boost::asio::posix::stream_descriptor key_input_;
  char key_pressed_;

  publish::RemoteDispatcherManager remote_dispatch_manager_;
  publish::LocalDispatcherManager publish_manager_;

  procedure::LocalDispatcherManager procedure_manager_;

  NodeLinkManager node_link_manager_;

public:
  NodeImpl() :
    //    signals_(io_service, SIGTERM, SIGINT), not in boost 1.40
    key_input_(io_service_),
    remote_dispatch_manager_(&io_service_),
    publish_manager_(&io_service_, &remote_dispatch_manager_),
    node_link_manager_(&io_service_)
  {
    // signals_.async_wait(boost::bind(&Node::quitHandler, this)); not in boost 1.40
    key_input_.assign( STDIN_FILENO );
    readKeyInput();
    // Link the dispatch handlers
    remote_dispatch_manager_.setLocalDispatchFunction( boost::bind( &publish::LocalDispatcherManager::receiveFromRemoteNode,
								    &publish_manager_, _1, _2 ) );
    // Register Publish Stuff
    node_link_manager_.registerPacketReceivedHandler( packet::Header::MSG_PACKET,
						      boost::bind( &publish::RemoteDispatcherManager::packetReceiveHandler,
								   &remote_dispatch_manager_, _1, _2 ) );
    remote_dispatch_manager_.setSendToNodeFunction( boost::bind( &NodeLinkManager::sendPacket,
								 &node_link_manager_, _1, _2, _3 ) );
  }

private:
  void quitHandler()
  {
    exit(0);
  }

  void keyPressedHandler( const boost::system::error_code& error, size_t bytes_transferred )
  {
    /*if ( error ) {
      std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
      return;
      }*/

    if ( key_pressed_ == 'q' )
    {
      exit(0);
    }

    readKeyInput();
  }

  void readKeyInput()
  {
    async_read( key_input_,
		boost::asio::buffer( &key_pressed_, sizeof(key_pressed_) ),
		boost::bind( &NodeImpl::keyPressedHandler,
			     this,
			     boost::asio::placeholders::error,
			     boost::asio::placeholders::bytes_transferred
			     )
		);
  }

  void run()
  {
    std::cout << "Running Node with ID " << node_link_manager_.getNodeID() << std::endl;
    io_service_.run();
  }

  publish::LocalDispatcherManager& getPublisherManager()
  {
    return publish_manager_;
  }

  procedure::LocalDispatcherManager& getProcedureManager()
  {
    return procedure_manager_;
  }

  void setNodeID( uint32_t node_id )
  {
    node_link_manager_.setNodeID(node_id);
  }

  void accept( const std::string& url )
  {
    node_link_manager_.accept(url);
  }

  void connect( uint32_t remote_node_id, const std::string& url )
  {
    node_link_manager_.connect(remote_node_id, url);
  }


};

}

#endif
