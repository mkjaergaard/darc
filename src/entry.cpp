#include <darc/entry.hpp>

namespace beam
{

template<>
std::string to_string(const arg<darc::entry>& e)
{
  return e->name;
}

}
