#pragma once
#include <iostream>
#include <vector>
#include <exception>
#include <map>
#include <unordered_map>
#include <docxpreparer/filters/base_parser.h>
#include <docxpreparer/filters/tag_find_parser.h>
#include <docxpreparer/filters/position_parser.h>

namespace filters
{

namespace parsers
{
  
class istream_tags_filter
{
private:
  enum class field_status {
    empty,
    field
  };

  struct flc_tags_information
  {
    std::string value;
    bool have_value = false;
  };

  struct flc_tags_position_information
  {
    std::streampos position;
    size_t tag_size;
  };


private:
  std::string get_tag(std::streamsize position, const std::string& tag)
  {
    auto& text = tags_info_[tag];
    tags_position_info_[position - tag.size()] = tag;
    if (!text.have_value)
    {
      text.value = "{tag}" + tag + "{/tag}";
    }
    return text.value;
  }

  std::string get_tag_by_position(std::streamsize position)
  {
    return tags_info_[tags_position_info_[position]].value;
  }

  std::vector<int> convert_to_codes(const std::string& str)
  {
    return std::vector<int>(std::begin(str), std::end(str));
  }

  std::vector<int> replace_and_fill_positions_tag(std::streamsize position, const std::vector<int>& tag)
  {
    return convert_to_codes(get_tag(position, std::string(std::begin(tag), std::end(tag))));
  }

  std::vector<int> get_tag_by_position(std::streamsize position, const std::vector<int>& tag)
  {
    return convert_to_codes(get_tag_by_position(position));
  }

  flc_tags_position_information get_next_tag_position(std::streampos current_position)
  {
    auto res = std::lower_bound(std::begin(tags_position_info_), std::end(tags_position_info_), current_position, [](const auto ps_pair, auto current_position) -> bool {return ps_pair.first < current_position; });
    if (res == std::end(tags_position_info_))
    {
      return flc_tags_position_information{ std::streampos(-1), 0 };
    }
    return flc_tags_position_information{ res->first, res->second.size() };
  }

private:
  struct flc_byte_out :
    public tag_find_parser
  {
  public:
    flc_byte_out(istream_tags_filter& parent) :
      parent_(parent)
    {}

  public:
    std::vector<int> replace(std::basic_istream<char>& in, const std::vector<int>& r) override
    {
      return parent_.replace_and_fill_positions_tag(in.tellg(), r);
    }

  private:
    istream_tags_filter& parent_;

  };

  struct flc_position_out :
    public position_parser
  {
  public:
    flc_position_out(istream_tags_filter& parent) :
      parent_(parent)
    {}

  public:
    std::vector<int> replace(std::basic_istream<char>& in, const std::vector<int>& r) override
    {
      return parent_.get_tag_by_position(in.tellg(), r);
    }

    virtual void get_next_position(std::streampos curr_position)
    {
      auto inf = parent_.get_next_tag_position(curr_position);
      set_next_position(inf.tag_size, inf.position);
    }

  private:
    istream_tags_filter& parent_;

  };

public:
  istream_tags_filter() :
    tags_info_(),
    status_(field_status::empty)
  {}

private:
  std::shared_ptr<base_parser> make_writer()
  {
    if (status_ == field_status::field)
    {
      return std::shared_ptr<base_parser>(new flc_position_out(*this));
    }
    return std::shared_ptr<base_parser>(new flc_byte_out(*this));
  }

public:
  int process(std::istream& in)
  {
    if (writer_ == nullptr)
    {
      writer_ = make_writer();
    }
    int c;
    c = writer_->parse(in);
    if (c == EOF)
    {
      writer_ = nullptr;
      status_ = field_status::field;
    }

    return c;
  }

public:
  void change_tag(const std::string& tag, const std::string& value)
  {
    tags_info_[tag] = flc_tags_information{ value, true };
  }

private:
  std::unordered_map<std::string, flc_tags_information> tags_info_;
  std::map<std::streampos, std::string> tags_position_info_;
  field_status status_;
  std::shared_ptr<base_parser> writer_;
};

}
}
