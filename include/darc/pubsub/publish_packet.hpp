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
  std::string topic_name;
  std::string type_name;

  publish_packet(const darc::ID& topic_id = ID::null(),
                 const std::string& topic_name = "",
                 const std::string& type_name = "") :
    topic_id(topic_id),
    topic_name(topic_name),
    type_name(type_name)
  {
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & topic_id;
    ar & topic_name;
    ar & type_name;
  }

};

}
}
