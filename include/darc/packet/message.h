#ifndef __DARC_PACKET_MESSAGE_H_INCLUDED__
#define __DARC_PACKET_MESSAGE_H_INCLUDED__

#include <darc/packet/parser.h>
#include <darc/serialized_message.h>

namespace darc
{
namespace packet
{

struct Message//Header
{
  std::string topic;

  Message()
  {
  }

  Message(const std::string& topic) :
    topic(topic)
  {
  }

  size_t read( const uint8_t * data, size_t data_len )
  {
    // Topic
    return Parser::readString(topic, data, data_len);
  }

  size_t write( uint8_t * data, size_t size )
  {
    return Parser::writeString(topic.c_str(), data, size);
  }

};

} // namespace packet
} // namespace darc

#endif
