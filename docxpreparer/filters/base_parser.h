#pragma once
#include <iostream>
#include <vector>
#include <exception>


namespace filters
{

namespace parsers
{

namespace exceptions
{

class needle_flush_cache_exception:
  public std::exception
{
public:
  const char* what() const noexcept override {
    return "needle flush cache exception";
  }
};

class cant_flush_empty_buffer_exception :
  public std::exception
{
public:
  const char* what() const noexcept override {
    return "can't flush empty buffer exception";
  }
};

class end_of_stream_exception :
  public std::exception
{
public:
  const char* what() const noexcept override {
    return "end of stream exception";
  }
};

}

extern const size_t max_tag_size;

enum class state
{
  find_tag_start,
  find_tag_start_twice,
  find_tag_finish,
  stop
};


class base_parser
{
public:
  base_parser() :
    cached_(),
    pos_(0)
  {}

public:

  int parse(std::istream& in)
  {
    int res = parse_release(in);
    if (res == EOF)
    {
      clear();
      cache_clear();
    }
    return res;
  }

  char get_next_char(std::istream& in)
  {
    int code = in.get();
    if (code == EOF)
    {
      cache_save(code);
      throw exceptions::end_of_stream_exception();
    }

    return static_cast<char>(code);
  }

  int parse_release(std::istream& in)
  {
    try {
      if (!is_cache_empty())
      {
        return cache_flush();
      }
      return get_next(in);
    }
    catch (const exceptions::needle_flush_cache_exception&)
    {
      return cache_flush();
    }

  }

  int cache_flush()
  {
    if (!cached_.empty())
    {
      char c = cached_[pos_++];
      if (pos_ == cached_.size())
      {
        cache_clear();
      }
      return c;
    }
    throw exceptions::cant_flush_empty_buffer_exception();
  }

  bool is_cache_empty()
  {
    return cached_.empty();
  }

  bool is_cache_overflow(size_t max_tag_size)
  {
    return cached_.size() >= max_tag_size;
  }

  void cache_save(int c)
  {
    cached_.push_back(c);
  }

  void cache_clear()
  {
    pos_ = 0;
    cached_.clear();
  }

  void cache_replace(std::istream& in)
  {
    cached_ = replace(in, cached_);
  }

  void go_to_fulsh_ceche_if_needle()
  {
    if (!is_cache_empty())
    {
      throw exceptions::needle_flush_cache_exception();
    }
  }

public:
  virtual void clear() = 0;
  virtual int get_next(std::istream& in) = 0;
  virtual std::vector<int> replace(std::basic_istream<char>& in, const std::vector<int>&) = 0;

private:
  std::vector<int> cached_;
  size_t pos_;
};

}
}
