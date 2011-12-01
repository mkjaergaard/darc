#ifndef __DARC_NODE_H_INCLUDED__
#define __DARC_NODE_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <darc/local_dispatcher.h>
#include <darc/subscriber_impl.h>
#include <darc/publisher_impl.h>
#include <darc/remote_dispatch_handler.h>
#include <darc/local_dispatch_handler.h>

#include <std_msgs/String.h>

namespace darc
{

class Node : public boost::enable_shared_from_this<Node>
{
public:
  typedef boost::shared_ptr<Node> Ptr;
  
private:
public: // tmp
  boost::asio::io_service io_service_;

  //  boost::asio::signal_set signals_; not in boost 1.40

  boost::asio::posix::stream_descriptor key_input_;
  char key_pressed_;

  RemoteDispatchHandler remote_dispatch_handler_;
  LocalDispatchHandler local_dispatch_handler_;

  typedef std::map<const std::string, boost::shared_ptr<LocalDispatcherAbstract> > LocalDispatcherListType;
  LocalDispatcherListType local_dispatcher_list_;

public:
  Node() :
    //    signals_(io_service, SIGTERM, SIGINT), not in boost 1.40
    key_input_(io_service_),
    remote_dispatch_handler_(&io_service_),
    local_dispatch_handler_(&io_service_, &remote_dispatch_handler_)
  {
    // signals_.async_wait(boost::bind(&Node::quitHandler, this)); not in boost 1.40
    key_input_.assign( STDIN_FILENO );
    readKeyInput();
    // Link the dispatch handlers
    remote_dispatch_handler_.setLocalDispatchFunction( boost::bind( &LocalDispatchHandler::receiveFromRemoteNode,
								    &local_dispatch_handler_, _1, _2 ) );
  }

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
		boost::bind( &Node::keyPressedHandler,
			     this,
			     boost::asio::placeholders::error,
			     boost::asio::placeholders::bytes_transferred
			     )
		);
  }

  void doSomeFun()
  {
    /*
    boost::shared_ptr<boost::thread> thread(new boost::thread( boost::bind(&darc::Node::run, this)));
    Instance()->threads_.push_back(thread);

    udp1_.addRemoteNode( 0, "127.0.0.1", "19001");

    boost::shared_ptr<SerializedMessage> msg_s( new SerializedMessage() );
    boost::shared_ptr<std_msgs::String> b( new std_msgs::String() );
    b->data = "test";
    msg_s->serializeMsg(b);
    
    udp1_.dispatch( 0, msg_s);
    */
  }

  void run()
  {
    std::cout << "Running Node" << std::endl;
    io_service_.run();
  }

  // called by the Subscriber
  // todo: not thread safe
  template<typename T>
  void RegisterSubscriber( const std::string& topic, boost::shared_ptr<SubscriberImpl<T> > sub )
  {
    local_dispatch_handler_.RegisterSubscriber<T>(topic, sub);
  }

  // Called by Publisher
  // todo: not thread safe
  template<typename T>
  void RegisterPublisher( const std::string& topic, boost::shared_ptr<PublisherImpl<T> > pub )
  {
    local_dispatch_handler_.RegisterPublisher<T>(topic, pub);
  }

};

}

#endif
