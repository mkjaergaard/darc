/*
 * Copyright (c) 2012, Prevas A/S
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Prevas A/S nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * DARC Parameter Value Impl class
 *
 * \author Morten Kjaergaard
 */

#ifndef __PARAMETER_VALUE_IMPL_H_INCLUDED__
#define __PARAMETER_VALUE_IMPL_H_INCLUDED__

#include <string>
#include <yaml-cpp/yaml.h>
#include <boost/shared_ptr.hpp>

#include <smart_cfg_server/parameter_value_abstract.h>
#include <smart_cfg_server/parameter_status.h>
#include <smart_cfg_server/parameter_types.h>

class UnsetValue {};

template<typename T = UnsetValue>
class ParameterValueImpl : public ParameterValueAbstract
{
public:
  typedef boost::shared_ptr<ParameterValueAbstract> AbstractPtr;

protected:
  T value_;
  ParameterStatus status_;

public:
  ParameterValueImpl() :
    value_(),
    status_( ParameterStatus::UNKNOWN )
  {
  }

  ParameterValueImpl(const T& value, const ParameterStatus::Type status = ParameterStatus::UNKNOWN) :
    value_(value),
    status_( status )
  {
  }

  ParameterValueImpl(const ParameterStatus::Type status) :
    value_(),
    status_( status )
  {
  }

  static boost::shared_ptr<ParameterValueAbstract> createFromType( const std::string& type )
  {
    if( type == ParameterTypes::TYPE_int )
    {
      return boost::shared_ptr<ParameterValueAbstract>( new ParameterValueImpl<int>() );
    }
    else if( type == ParameterTypes::TYPE_float )
    {
      return boost::shared_ptr<ParameterValueAbstract>( new ParameterValueImpl<double>() );
    }
    else // if( type == ParameterTypes::TYPE_unknown )
    {
      return boost::shared_ptr<ParameterValueAbstract>( new ParameterValueImpl<UnsetValue>() );
    }
  }

  T& value()
  {
    return value_;
  }

  const T& value() const
  {
    return value_;
  }

  const ParameterStatus::Type& getStatus() const
  {
    return status_.status();
  }

  bool isKnown() const
  {
    return status_.isKnown();
  }

  virtual const std::string& getType() const
  {
    return ParameterTypes::typeOf<T>();
  }

  virtual const std::string getEncodedValue() const
  {
    YAML::Emitter yaml_encoded;
    yaml_encoded << value_ << " ";
    return std::string(yaml_encoded.c_str());
  }

  virtual void setFromEncodedValue( const std::string& encoded_value )
  {
    std::cout << ":" << encoded_value << ":" << std::endl;
    // todo: do some try/catch
    std::stringstream encoded_stream(encoded_value);
    YAML::Parser parser(encoded_stream);

    YAML::Node node;
    if (parser.GetNextDocument(node))
    {
      node >> value_;
    }
    else
    {
      // return false;
      assert(false);
    }
  }

};

// Specialization for UnsetValue

template<>
const std::string& ParameterValueImpl<UnsetValue>::getType() const;

template<>
const std::string ParameterValueImpl<UnsetValue>::getEncodedValue() const;

template<>
void ParameterValueImpl<UnsetValue>::setFromEncodedValue( const std::string& encoded_value );


#endif
