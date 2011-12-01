#ifndef __DARC_SHARED_BUFFER_H_INCLUDED__
#define __DARC_SHARED_BUFFER_H_INCLUDED__

#include <boost/shared_array.hpp>

class SharedBuffer : private boost::shared_array<uint8_t>
{
private:
  size_t size_;

  SharedBuffer( size_t size ) :
    boost::shared_array<uint8_t>( new uint8_t[size] ),
    size_(size)
  {
  }  

public:
  uint8_t * data() const
  {
    return boost::shared_array<uint8_t>::get();
  }

  size_t size() const
  {
    return size_;
  }

  static SharedBuffer create( size_t size )
  {
    return SharedBuffer( size );
  }

};

#endif
