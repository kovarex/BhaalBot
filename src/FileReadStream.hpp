#pragma once
#include <ReadStream.hpp>
#include <Filesystem.hpp>
#include <stdio.h>

class FileReadStream : public ReadStream
{
public:
  FileReadStream(const Filesystem::Path& path) { this->init(path); }
  // requires explicit init call
  FileReadStream() : fp(nullptr) {}
  ~FileReadStream();

  void init(const Filesystem::Path& path);

  uint32_t read(char* buffer, uint32_t size) override;
  uint64_t ftell();
  void seek(uint64_t position);
  bool eof() const override;

  Filesystem::Path getFilename() { return filename; }
protected:
  FileReadStream(FILE* fp)
    : fp(fp) {}

  Filesystem::Path filename;
  FILE* fp;
};
