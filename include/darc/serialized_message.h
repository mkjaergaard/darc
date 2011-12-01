#ifndef __DARC_SERIALIZED_MESSAGE_H_INCLUDED_
#define __DARC_SERIALIZED_MESSAGE_H_INCLUDED_

#include <boost/shared_ptr.hpp>
#include <ros/serialization.h>
#include <darc/shared_buffer.h>

namespace darc
{

class SerializedMessage
{
public:
  typedef boost::shared_ptr<SerializedMessage> Ptr;
  typedef const boost::shared_ptr<const SerializedMessage> ConstPtr;

  SharedBuffer buffer_;
  size_t data_size_;

public:
  SerializedMessage() :
    buffer_( SharedBuffer::create(10) ),
    data_size_(0)
  {
  }

  SerializedMessage( SharedBuffer buffer, size_t data_size ) :
    buffer_(buffer),
    data_size_(data_size)
  {
  }

  template<typename T>
  explicit SerializedMessage( const boost::shared_ptr<const T> &msg ) :
    buffer_( SharedBuffer::create(10) ),
    data_size_(0)
  {
    serializeMsg(msg);
  }

  template<typename T>
  uint32_t serializeMsg(boost::shared_ptr<T> msg)
  {
    // Allocate proper buffer size
    data_size_ = 1024;//ros::serialization::Serializer<T>::serializedLength(*(msg.get()));
    buffer_ = SharedBuffer::create( data_size_ );
    // Serialize into buffer
    ros::serialization::OStream ostream(buffer_.data(), buffer_.size());
    ros::serialization::serialize( ostream, *(msg.get()) );
    return data_size_;
  }

  template<typename T>
  boost::shared_ptr<T> deserialize() const
  {
    boost::shared_ptr<T> msg( new T() );
    ros::serialization::IStream in( buffer_.data(), buffer_.size() );
    ros::serialization::deserialize(in, *(msg.get()) );
    return msg;
  }

  const SharedBuffer& getBuffer() const
  {
    return buffer_;
  }

};

}

#endif
