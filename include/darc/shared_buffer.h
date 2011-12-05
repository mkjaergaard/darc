#ifndef __DARC_SHARED_BUFFER_H_INCLUDED__
#define __DARC_SHARED_BUFFER_H_INCLUDED__

#include <boost/shared_array.hpp>

class SharedBuffer : private boost::shared_array<uint8_t>
{
private:
  size_t size_;
  size_t start_offset_;

  SharedBuffer( size_t size ) :
    boost::shared_array<uint8_t>( new uint8_t[size] ),
    size_(size),
    start_offset_(0)
  {
  }  

public:
  uint8_t * data() const
  {
    return boost::shared_array<uint8_t>::get() + start_offset_;
  }

  size_t size() const
  {
    return size_ - start_offset_;
  }

  void addOffset(size_t offset)
  {
    start_offset_ += offset;
  }

  static SharedBuffer create( size_t size )
  {
    return SharedBuffer( size );
  }

};

#endif
