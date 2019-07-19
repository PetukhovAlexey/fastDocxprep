#pragma once

#include <ZipLib/ZipFile.h>
#include <ZipLib/streams/memstream.h>

#include <ZipLib/methods/Bzip2Method.h>

#include <fstream>
#include <string>

namespace docxpreparer
{
  struct editable_document
  {
    ZipArchive::Ptr archive;
  };

  inline void print_archive_info(const std::string& path)
  {
    ZipArchive::Ptr archive = ZipFile::Open(path);
    size_t entries = archive->GetEntriesCount();

    printf("[o] Listing archive (comment: '%s'):\n", archive->GetComment().c_str());
    printf("[o] Entries count: %lu\n", entries);

    for (size_t i = 0; i < entries; ++i)
    {
      auto entry = archive->GetEntry(int(i));

      printf("[o] -- %s\n", entry->GetFullName().c_str());
      printf("[o]   >> uncompressed size: %lu\n", entry->GetSize());
      printf("[o]   >> compressed size: %lu\n", entry->GetCompressedSize());
      printf("[o]   >> password protected: %s\n", entry->IsPasswordProtected() ? "yes" : "no");
      printf("[o]   >> compression method: %s\n", entry->GetCompressionMethod() == DeflateMethod::CompressionMethod ? "DEFLATE" : "stored");
      printf("[o]   >> comment: %s\n", entry->GetComment().c_str());
      printf("[o]   >> crc32: 0x%08X\n", entry->GetCrc32());
    }

    printf("\n");
  }
}
