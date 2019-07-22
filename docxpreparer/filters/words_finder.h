#pragma once
#include <iostream>
#include <vector>
#include <exception>
#include <docxpreparer/filters/base_parser.h>


namespace filters
{

namespace parsers
{

class word_finder
{

public:
  word_finder(const std::string& word) :
    word_(word),
    locked_byte_(0)
  {
  }

  bool check(std::streampos cur_pos, char c)
  {
    compiled_.push_back(c);
    if (compiled_.size() <= word_.size())
    {
      if (std::equal(std::begin(compiled_), std::end(compiled_), std::begin(word_)))
      {
        locked_byte_ = compiled_.size() == 1 ? cur_pos : locked_byte_;
        return compiled_.size() == word_.size();
      }
    }
    compiled_.clear();
    locked_byte_ = 0;
    return false;
  }

private:
  std::string compiled_;
  std::string word_;
  std::streampos locked_byte_;

};

}
}
