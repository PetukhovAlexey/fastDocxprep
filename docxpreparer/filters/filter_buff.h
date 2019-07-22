#pragma once
#include <iostream>
#include <vector>
#include <exception>
#include <docxpreparer/filters/base_parser.h>
#include <docxpreparer/filters/istream_filter.h>


namespace filters
{

namespace parsers
{


class my_streambuf :
  public std::basic_streambuf<char, std::char_traits<char> >
{
  typedef std::basic_streambuf<char, std::char_traits<char> > base;

public:
  my_streambuf(std::basic_istream<char>* original) :
    original_(original),
    buffer{ '\0' }
  {
  }
  my_streambuf(std::basic_istream<char>& original) :
    original_(&original),
    buffer{ '\0' }
  {
  }

protected:
  virtual int_type underflow() { // get a character from stream, but don't point past it
    size_t i;
    for (i = 0; i < sizeof(buffer) / sizeof(*buffer) && (*original_); ++i)
    {
      int_type t = filter_.process(*original_);
      if (t == std::char_traits<char>::eof())
      {
        break;
      }
      buffer[i] = t;
    }
    base::setg(buffer, buffer, buffer + i);
    return (*original_) || i != 0 ? buffer[0] : std::char_traits<char>::eof();
  }

private:
  std::basic_istream<char>* original_;
  istream_tags_filter filter_;
  char buffer[1024];
};


}
}
