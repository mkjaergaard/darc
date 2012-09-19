#include <gtest/gtest.h>

#include <darc/distributed_container/shared_set.hpp>

#include <boost/bind.hpp>
#include <llog/logger.hpp>

class SharedSetTest : public testing::Test
{
public:

};

TEST(SharedSetTest, Subscribe)
{
  darc::distributed_container::shared_set<std::string, uint32_t> my_vector;

}
