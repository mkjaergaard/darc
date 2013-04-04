#pragma once

#include <darc/id.hpp>

namespace darc
{

struct RemoteID
{
  peer_id;
  id;

  RemoteID(const ID& peer_id, const ID& id) :
    peer_id(id),
    id(id)
};

}
