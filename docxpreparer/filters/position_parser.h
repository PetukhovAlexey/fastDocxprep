#pragma once
#include <iostream>
#include <vector>
#include <exception>
#include <docxpreparer/filters/base_parser.h>


namespace filters
{

namespace parsers
{

class position_parser :
  public base_parser
{
public:
  position_parser() :
    next_tag_pos_(0),
    next_tag_size_(0)
  {
  }

public:
  virtual void clear() override
  {
    next_tag_pos_ = 0;
  }

  virtual int get_next(std::istream& in) override
  {
    if (next_tag_pos_ == std::streampos(0))
    {
      get_next_position(next_tag_pos_);
    }

    if (in.tellg() == next_tag_pos_)
    {
      cache_replace(in);
      in.seekg(next_tag_size_, std::ios::cur);
      get_next_position(next_tag_pos_ += 1);
      go_to_fulsh_ceche_if_needle();
    }
    return get_next_char(in);
  }

  virtual void get_next_position(std::streampos curr_position) = 0;

  virtual void set_next_position(size_t tag_size, std::streampos pos)
  {
    next_tag_pos_ = pos;
    next_tag_size_ = tag_size;
  }

private:
  std::streampos next_tag_pos_;
  size_t next_tag_size_;

};

}
}
