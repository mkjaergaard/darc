#pragma once

#include <darc/id.hpp>

namespace asmsg
{

struct message_packet
{
  const static uint32_t payload_id = 0x02;

  darc::ID topic_id;

  message_packet(const darc::ID& topic_id) :
    topic_id(topic_id)
  {
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & topic_id;
  }

};

}
