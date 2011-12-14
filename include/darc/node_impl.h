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

  publish::RemoteDispatcherManager remote_dispatch_handler_;
  publish::LocalDispatcherManager publish_manager_;

  procedure::LocalDispatcherManager procedure_manager_;

  NodeLinkManager node_link_manager_;

public:
  NodeImpl() :
    //    signals_(io_service, SIGTERM, SIGINT), not in boost 1.40
    key_input_(io_service_),
    remote_dispatch_handler_(&io_service_),
    publish_manager_(&io_service_, &remote_dispatch_handler_),
    node_link_manager_(&io_service_)
  {
    // signals_.async_wait(boost::bind(&Node::quitHandler, this)); not in boost 1.40
    key_input_.assign( STDIN_FILENO );
    readKeyInput();
    // Link the dispatch handlers
    remote_dispatch_handler_.setLocalDispatchFunction( boost::bind( &publish::LocalDispatcherManager::receiveFromRemoteNode,
								    &publish_manager_, _1, _2 ) );
    // Register
    node_link_manager_.registerPacketReceivedHandler( packet::Header::MSG_PACKET,
						      boost::bind( &publish::RemoteDispatcherManager::packetReceiveHandler,
								   &remote_dispatch_handler_, _1, _2 ) );
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
    std::cout << "Running Node" << std::endl;
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
