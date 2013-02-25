#pragma once

#include <darc/id.hpp>

namespace darc
{
namespace pubsub
{

struct publish_packet
{
  const static uint32_t payload_id = 0x04;

  darc::ID topic_id;

  publish_packet(const darc::ID& topic_id) :
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
}
