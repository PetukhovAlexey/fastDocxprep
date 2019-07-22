#pragma once
#include <iostream>
#include <vector>
#include <exception>
#include <docxpreparer/filters/base_parser.h>


namespace filters
{

namespace parsers
{

class tag_find_parser :
  public base_parser
{
public:
  tag_find_parser() :
    cur_state_(state::find_tag_start)
  {}

public:

  void clear() override
  {
    cur_state_ = state::find_tag_start;
  }


  int get_next(std::istream& in) override
  {
    do {
      char c = get_next_char(in);

      switch (cur_state_)
      {
      case state::find_tag_start:
        if (c == '[')
        {
          cache_save(c);
          cur_state_ = state::find_tag_finish;
        }
        else
        {
          return c;
        }
        break;
      case  state::find_tag_start_twice:
        cache_save('[');
        cur_state_ = state::find_tag_finish;
      case state::find_tag_finish:
        if (c == '[')
        {
          cur_state_ = state::find_tag_start_twice;
          go_to_fulsh_ceche_if_needle();
        }
        else if (c == ']')
        {
          cache_save(']');
          cache_replace(in);
          cur_state_ = state::find_tag_start;
          go_to_fulsh_ceche_if_needle();
        }
        else
        {
          cache_save(c);
          if (is_cache_overflow(max_tag_size))
          {
            cur_state_ = state::find_tag_start;
            go_to_fulsh_ceche_if_needle();
          }
        }
        break;
      }
    } while (cur_state_ != state::stop);
    return EOF;
  }

public:

private:
  state cur_state_;
};

}
}
