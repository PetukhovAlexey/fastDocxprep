// fastDocxprep.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#pragma warning(disable:4996)
#include <iostream>

#include <docxpreparer/docxpreparer.h>
#include <boost/iostreams/char_traits.hpp> // EOF, WOULD_BLOCK
#include <boost/iostreams/concepts.hpp>    // input_filter
#include <boost/iostreams/operations.hpp>  // get

std::ostream& operator<< (std::ostream& ostr, std::istream& istr)
{
  return ostr << istr.rdbuf();
}


const size_t max_tag_size = 40;

enum class state
{
  find_tag_start,
  find_tag_start_twice,
  find_tag_finish,
  stop
};

class cant_flush_empty_buffer
{
};

class end_of_stream_exception
{
};

class find_tag_start_twice_exception
{
};

class cashe_overflow_exception
{
};

class print_replaced_tag_exception
{
};

class flc_byte_out_base
{
public:
  flc_byte_out_base() :
    cached_(),
    pos_(0),
    cur_state_(state::find_tag_start)
  {}

public:

  int flush_cache()
  {
    if (!cached_.empty())
    {
      char c = cached_[pos_++];
      if (pos_ == cached_.size())
      {
        cached_.clear();
        pos_ = 0;
      }
      return c;
    }
    throw cant_flush_empty_buffer();
  }

  int process(std::istream& in)
  {
    try {
      if (!cached_.empty())
      {
        return flush_cache();
      }
      return get_next(in);
    }
    catch (const cant_flush_empty_buffer&)
    {
      return get_next(in);
    }
    catch (const end_of_stream_exception&)
    {
      return flush_cache();
    }
    catch (const find_tag_start_twice_exception&)
    {
      return flush_cache();
    }
    catch (const cashe_overflow_exception&)
    {
      return flush_cache();
    }
    catch (const print_replaced_tag_exception&)
    {
      return flush_cache();
    }

  }


  int get_process(std::istream& in)
  {
    try {
      get_next(in);
      return flush_cache();
    }
    catch (const cant_flush_empty_buffer&)
    {
    }
  }

  int get_next(std::istream& in)
  {
    do {
      int code = in.get();
      if (code == EOF)
      {
        cached_.push_back(code);
        throw end_of_stream_exception();
      }

      char c = static_cast<char>(code);

      switch (cur_state_)
      {
      case state::find_tag_start:
        if (c == '[')
        {
          cached_.push_back(c);
          cur_state_ = state::find_tag_finish;
        }
        else
        {
          return c;
        }
        break;
      case  state::find_tag_start_twice:
        cached_.push_back('[');
        cur_state_ = state::find_tag_finish;
      case state::find_tag_finish:
        if (c == '[')
        {
          cur_state_ = state::find_tag_start_twice;
          throw find_tag_start_twice_exception();
        }
        else if (c == ']')
        {
          cached_.push_back(']');
          cached_ = replace(cached_);
          cur_state_ = state::find_tag_start;
          throw print_replaced_tag_exception();
        }
        else
        {
          cached_.push_back(c);
          if (cached_.size() >= max_tag_size)
          {
            cur_state_ = state::find_tag_start;
            throw cashe_overflow_exception();
          }
        }
        break;
      }
    } while (cur_state_ != state::stop);
  }

public:
  virtual std::vector<int> replace(const std::vector<int>&) = 0;

private:
  std::vector<int> cached_;
  size_t pos_;
  state cur_state_;
};

class flc_tags_filter
{

private:
  struct flc_byte_out : public flc_byte_out_base
  {
    std::vector<int> replace(const std::vector<int>& r)
    {
      std::string pref("<tag>");
      std::string postf("</tag>");
      std::vector<int> res(pref.begin(), pref.end());
      res.insert(res.end(), r.begin(), r.end());
      res.insert(res.end(), postf.begin(), postf.end());
      return res;
    }
  };

public:
  flc_tags_filter(std::ostream& out, std::istream & in)
    :out_(out), in_(in)
  {}

  std::string replace(std::string tag)
  {
    return "<tag>" + tag + "</tag>";
  }

  void process()
  {
    flc_byte_out writer;
    int c;
    do
    {
      c = writer.process(in_);
      if (c != EOF)
      {
        out_.put(c);
      }
    } while (c != EOF);
  }

private:
  std::ostream& out_;
  std::istream& in_;
};

int main()
{
  /*
  ZipArchive::Ptr archive = ZipFile::Open("test.docx");


  ZipArchiveEntry::Ptr old_documentEntry = archive->GetEntry("word/document.xml");

  old_documentEntry->SetFullName("word/old_document.xml");
  std::istream *stream_data = old_documentEntry->GetDecompressionStream();

  if (stream_data != nullptr)
  {

    ZipArchiveEntry::Ptr entry = archive->CreateEntry("word/document.xml");
    if (entry != nullptr)
    {

      entry->SetCompressionStream(
        *stream_data
      );
    }
  }
  else {
    std::cerr << "read stream error" << std::endl;
  }

  archive->RemoveEntry("word/old_document.xml");
  std::ofstream result("testres.docx", std::ios::binary);
  archive->WriteToStream(result);
  */

  flc_tags_filter filter(std::cout, std::cin);
  filter.process();

  return 0;
}