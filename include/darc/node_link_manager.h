#ifndef __DARC_NODE_LINK_MANAGER_H_INCLUDED__
#define __DARC_NODE_LINK_MANAGER_H_INCLUDED__

#include <boost/xpressive/xpressive.hpp>
#include <darc/udp/link_manager.h>
#include <darc/packet/header.h>

namespace xp = boost::xpressive;

namespace darc
{

class NodeLinkManager
{
private:
  uint32_t node_id_;

  // Regular Expression to parse urls
  const xp::sregex url_rex_;

  // Protocol handlers
  typedef std::map<const std::string, LinkManagerAbstract*> ManagerMapType;
  ManagerMapType manager_map_;
  // -
  udp::LinkManager udp_manager_;

  // List of links
  //  Connections (Outgoing)
  typedef std::map< uint32_t, NodeLink::Ptr > ConnectionListType;
  ConnectionListType connection_list_;
  //  Acceptors (Incoming)
  typedef std::vector< NodeLink::Ptr > AcceptorListType;
  AcceptorListType acceptor_list_;

  // Callbacks
  typedef boost::function< void(SharedBuffer, std::size_t) > PacketReceivedHandlerType;
  std::map< packet::Header::PayloadType, PacketReceivedHandlerType > packet_received_handlers_;

public:
  NodeLinkManager( boost::asio::io_service * io_service ) :
    node_id_(0xFFFF),
    url_rex_( (xp::s1= +xp::_w) >> "://" >> (xp::s2= +~xp::_n) ),
    udp_manager_( io_service )
  {
    // Link protocol names and protocol handlers
    manager_map_["udp"] = &udp_manager_;
  }

  void registerPacketReceivedHandler( packet::Header::PayloadType type, PacketReceivedHandlerType handler )
  {
    packet_received_handlers_[type] = handler;
  }

  void accept( const std::string& url )
  {
    NodeLink::Ptr link = createFromAccept(url);
    acceptor_list_.push_back( link );
    //? link->setReceiveCallback( boost::bind(&RemoteDispatcherManager::receiveFromRemoteNode, this, _1, _2, _3) );
    link->setReceiveCallback( boost::bind(&NodeLinkManager::receiveHandler, this, _1, _2) );
  }

  void connect( uint32_t remote_node_id, const std::string& url )
  {
    NodeLink::Ptr link = createFromConnect(remote_node_id, url);
    connection_list_[remote_node_id] = link;
    link->setNodeID( node_id_ );
  }

  // todo: create one automatically
  void setNodeID( uint32_t node_id )
  {
    node_id_ = node_id;
    // todo: update alle existing links
  }

private:
  void receiveHandler( SharedBuffer buffer, std::size_t data_len )
  {
    packet::Header header;
    header.read( buffer.data(), data_len );
    buffer.addOffset( packet::Header::size() );
    data_len -= packet::Header::size();

    // Switch on packet type
    if (header.payload_type == packet::Header::MSG_PACKET)
    {
      packet_received_handlers_[packet::Header::MSG_PACKET]( buffer, data_len );
    }
    else
    {
      std::cout << "Unknown packet type received??" << std::endl;
    }
  }

  NodeLink::Ptr createFromAccept( const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      LinkManagerAbstract * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->accept(what[2]);
      }
      else
      {
	std::cout << "Unsupported protocol: " << what[1] << std::endl;
	return NodeLink::Ptr();
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
	return NodeLink::Ptr();
    }
  }

  NodeLink::Ptr createFromConnect( uint32_t remote_node_id, const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      LinkManagerAbstract * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->connect(remote_node_id, what[2]);
	return NodeLink::Ptr();
      }
      else
      {
	std::cout << "Unsupported protocol: " << what[1] << std::endl;
	return NodeLink::Ptr();
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
      return NodeLink::Ptr();
    }

  }

  // Get the correct protocol handler
  LinkManagerAbstract * getManager(const std::string& protocol)
  {
    ManagerMapType::iterator elem = manager_map_.find(protocol);
    if( elem != manager_map_.end() )
    {
      return elem->second;
    }
    else
    {
      return 0;
    }
  }

};

}

#endif
