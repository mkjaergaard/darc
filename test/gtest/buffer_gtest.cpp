#include <gtest/gtest.h>

#include <darc/buffer/raw_buffer.hpp>
#include <darc/buffer/const_size_buffer.hpp>
#include <darc/buffer/shared_buffer.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/make_shared.hpp>

#include <darc/network/outbound_data.hpp>
#include <darc/network/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

TEST(BufferTest, Raw)
{
  char data_1[1024];

  int val_1 = 99;

  darc::buffer::shared_buffer buffer_1 =
    boost::make_shared<darc::buffer::raw_buffer>(&data_1[0], 1024);
  std::ostream os_1(buffer_1->streambuf());

  boost::archive::binary_oarchive oarchive_1(os_1);

  oarchive_1 << val_1;

  val_1 = 0;

  darc::buffer::shared_buffer buffer_2 =
    boost::make_shared<darc::buffer::raw_buffer>(&data_1[0], 1024);

  std::istream is_1(buffer_2->streambuf());
  boost::archive::binary_iarchive iarchive_1(is_1);

  EXPECT_FALSE(val_1 == 99);
  iarchive_1 >> val_1;
  EXPECT_EQ(val_1, 99);

};

TEST(BufferTest, Stacked)
{
  char data_1[1024];

  uint32_t val_1 = 99;
  uint32_t val_2 = 120;

  // Create data
  darc::outbound_data<darc::serializer::boost_serializer, uint32_t> o_data_1(99);
  darc::outbound_data<darc::serializer::boost_serializer, uint32_t> o_data_2(120);
  darc::outbound_pair o_data(o_data_2, o_data_1);

  // Create buffer and pack data
  darc::buffer::shared_buffer buffer =
    boost::make_shared<darc::buffer::raw_buffer>(&data_1[0], 1024);
  o_data.pack(buffer);

  // Unpack data
  darc::inbound_data<darc::serializer::boost_serializer, uint32_t> in_val_2(buffer);
  darc::inbound_data<darc::serializer::boost_serializer, uint32_t> in_val_1(buffer);

  // Verify values
  EXPECT_EQ(in_val_1.get(),  99);
  EXPECT_EQ(in_val_2.get(), 120);

};

/*

#include <hns/distributed_header.hpp>

TEST(BufferTest, Stacked2)
{
  uint32_t val_1 = 99;
  uint32_t val_2 = 120;

  hns::header hdr;
  hdr.src_instance_id = hns::ID::create();
  hdr.dest_instance_id = hns::ID::create();
  hdr.payload_type = hns::header::control;

  // Create data
  hns::outbound_data<hns::boost_serializer, uint32_t> o_data_1(99);
  hns::outbound_data<hns::boost_serializer, hns::header> o_data_2(hdr);
  hns::outbound_pair o_data(o_data_2, o_data_1);

  // Create buffer and pack data
  hns::shared_buffer buffer = boost::make_shared<hns::const_buffer>(1024);
  o_data.pack(buffer);

  // Unpack data
  hns::inbound_data<hns::boost_serializer, hns::header> in_val_2(buffer);
  hns::inbound_data<hns::boost_serializer, uint32_t> in_val_1(buffer);

  // Verify values
  EXPECT_EQ(in_val_1.get(),  99);
  EXPECT_EQ(in_val_2.get().src_instance_id, hdr.src_instance_id);
  EXPECT_EQ(in_val_2.get().dest_instance_id, hdr.dest_instance_id);
  EXPECT_EQ(in_val_2.get().payload_type, 9);

};
*/

/*
TEST(BufferTest, Stacked)
{
  char data_1[1024];
  char data_2[1024];

  int val_1 = 99;
  int val_2 = 120;

  // Create first buffer
  hns::shared_buffer buffer_1 = boost::make_shared<hns::raw_buffer>(&data_1[0], 1024);

  EXPECT_EQ(buffer_1->data_count(), 0);
  EXPECT_TRUE(buffer_1->is_empty());

  std::ostream os_1(buffer_1->streambuf());
  boost::archive::binary_oarchive oarchive_1(os_1);

  buffer_1->mark_empty();
  EXPECT_TRUE(buffer_1->is_empty());

  oarchive_1 << val_1;

  EXPECT_FALSE(buffer_1->is_empty());

  // Stack another
  hns::shared_buffer buffer_2 =
    boost::make_shared<hns::raw_buffer>(&data_2[0], 1024, buffer_1);

  EXPECT_TRUE(buffer_2->is_empty());

  std::ostream os_2(buffer_2->streambuf());
  boost::archive::binary_oarchive oarchive_2(os_2);
  buffer_2->mark_empty();
  EXPECT_EQ(buffer_2->data_count(), 0);

  EXPECT_TRUE(buffer_2->is_empty());

  oarchive_2 << val_2;

  // Unwrap
  val_1 = 0;
  val_2 = 0;

  std::istream is_1(buffer_2->streambuf());
  boost::archive::binary_iarchive iarchive_1(is_1);

  EXPECT_EQ(buffer_2->data_count(), 0);

  EXPECT_FALSE(val_1 == 99);
  EXPECT_FALSE(val_2 == 120);
  iarchive_1 >> val_2;
  EXPECT_EQ(val_2, 120);

  EXPECT_TRUE(buffer_2->is_empty());

  buffer_2.flush();

  EXPECT_FALSE(buffer_2->is_empty());

  std::istream is_2(buffer_2->streambuf());
  boost::archive::binary_iarchive iarchive_2(is_2);

  iarchive_2 >> val_1;
  EXPECT_EQ(val_1, 99);

};
*/
