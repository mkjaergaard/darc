#ifndef __DARC_SERIALIZED_MESSAGE_H_INCLUDED_
#define __DARC_SERIALIZED_MESSAGE_H_INCLUDED_

#include <boost/shared_ptr.hpp>
#include <ros/serialization.h>

namespace darc
{

class SerializedMessage
{
public:
  uint8_t * data_;
  uint32_t buffer_size_;
  uint32_t data_size_;

public:
  SerializedMessage() :
    data_(0),
    buffer_size_(0),
    data_size_(0)
  {
  }

  ~SerializedMessage()
  {
    clear();
  }

  template<typename T>
  uint32_t serializeMsg(boost::shared_ptr<T> msg)
  {
    // Delete existing data
    clear();
    // Allocate proper buffer size
    data_size_ = ros::serialization::Serializer<T>::serializedLength(*(msg.get()));
    buffer_size_ = data_size_;
    data_ = new uint8_t(buffer_size_);
    // Serialize into buffer
    ros::serialization::OStream ostream(data_, buffer_size_);
    ros::serialization::serialize( ostream, *(msg.get()) );
    return data_size_;
  }

private:
  void clear()
  {
    if( data_ != 0 )
    {
      delete data_;
      data_ = 0;
    }
    buffer_size_ = 0;
    data_size_ = 0;
  }
};

}

#endif
