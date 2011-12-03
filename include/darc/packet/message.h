#ifndef __DARC_PACKET_MESSAGE_H_INCLUDED__
#define __DARC_PACKET_MESSAGE_H_INCLUDED__

#include <darc/packet/parser.h>
#include <darc/serialized_message.h>

namespace darc
{
namespace packet
{

struct Message
{
  std::string topic;
  SerializedMessage::Ptr message_data;

  size_t read( Shared_buffer data, size_t data_len )
  {
    // Topic
    std::string topic;
    size_t pos = Parser::readString(topic, data, data_len);
    // Actual Message
    message_data.reset( new SerializedMessage( data + pos, data_len - pos ) );
  }
};

} // namespace packet
} // namespace darc

#endif
