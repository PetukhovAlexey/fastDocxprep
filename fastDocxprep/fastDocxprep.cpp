// fastDocxprep.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#pragma warning(disable:4996)
#include <iostream>

#include <docxpreparer/docxpreparer.h>
#include <unordered_map>
#include <map>
#include <sstream>
#include <docxpreparer/filters/filter_buff.h>
#include <docxpreparer/filters/words_finder.h>

std::ostream& operator<< (std::ostream& ostr, std::istream& istr)
{
  return ostr << istr.rdbuf();
}


const size_t filters::parsers::max_tag_size = 40;


int main()
{

  //ZipArchive::Ptr archive = ZipFile::Open("test.docx");
  //
  //
  //ZipArchiveEntry::Ptr old_documentEntry = archive->GetEntry("word/document.xml");
  //
  //old_documentEntry->SetFullName("word/old_document.xml");
  //std::istream *stream_data = old_documentEntry->GetDecompressionStream();
  //
  //filters::parsers::my_streambuf filter_buf(stream_data);
  //std::istream basic_stream(
  //  &filter_buf);
  //if (stream_data != nullptr)
  //{
  //
  //  ZipArchiveEntry::Ptr entry = archive->CreateEntry("word/document.xml");
  //  if (entry != nullptr)
  //  {
  //    entry->SetCompressionStream(
  //      basic_stream
  //    );
  //  }
  //}
  //else {
  //  std::cerr << "read stream error" << std::endl;
  //}
  //
  //archive->RemoveEntry("word/old_document.xml");
  //std::ofstream result("testres.docx", std::ios::binary);
  //archive->WriteToStream(result);


  //test_process_by_string(std::cout, "text text\n[[[[[[tag]]]]]]][t] [tag1] [tag2000]\ntext");

  //std::string line;  
  //std::stringstream ss("text text\n[[[[[[tag]]]]]]][t] [tag1] [tag2000]\ntext");
  //my_streambuf filter_buf(ss);
  //std::istream sb(&filter_buf);
  //
  //
  //while (sb)
  //{
  //  std::getline(sb, line);
  //  if (sb)
  //  {
  //    std::cout << line << std::endl;
  //  }
  //}

  filters::parsers::word_finder wf("text");
  while (std::cin)
  {
    std::cout << wf.check(std::cin.tellg(),std::cin.get());
  }

  return 0;
}