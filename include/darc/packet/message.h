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

  size_t read( SharedBuffer buffer, size_t data_len )
  {
    // Topic
    std::string topic;
    size_t pos = Parser::readString(topic, buffer.data(), data_len);
    // Actual Message
    buffer.addOffset(pos);
    message_data.reset( new SerializedMessage( buffer, data_len - pos ) );
    return pos; //todo + msg size
  }
};

} // namespace packet
} // namespace darc

#endif
