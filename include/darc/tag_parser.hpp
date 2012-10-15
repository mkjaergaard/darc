#pragma once

#include <boost/tokenizer.hpp>

#include <stdexcept>

// move to external

namespace darc
{

struct invalid_tag_exception : public std::runtime_error
{
public:
  invalid_tag_exception(const std::string &err) : std::runtime_error(err)
  {
  }
};

//

class tag_parser
{
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer_type;
  boost::char_separator<char> seperator_;

  tokenizer_type tokenizer_;

public:
  typedef tokenizer_type iterator;

  typedef std::vector<std::string> namespace_list_type;

  namespace_list_type namespaces_;
  std::string tag_;

public:
  tag_parser(const std::string& full_tag) :
    seperator_("/", "", boost::keep_empty_tokens),
    tokenizer_(full_tag, seperator_)
  {
    for(tokenizer_type::iterator it = tokenizer_.begin();
	it != tokenizer_.end();
	it++)
    {
      namespaces_.push_back(*it);
    }

    if(namespaces_.empty() ||
       namespaces_.back() == "")
    {
      throw invalid_tag_exception(full_tag);
    }

    tag_ = namespaces_.back();
    namespaces_.pop_back();

  }

  const namespace_list_type& get_namespaces() const
  {
    return namespaces_;
  }

  const std::string& get_tag() const
  {
    return tag_;
  }

};

}
