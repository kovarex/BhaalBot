#pragma once
#include <WriteStream.hpp>
#include <Filesystem.hpp>
#include <stdio.h>

class FileWriteStream : public WriteStream
{
public:
  FileWriteStream(const Filesystem::Path& path, bool append = false);
  ~FileWriteStream();
  void write(const char* target, uint32_t size) override;
  void flush() override;
  void seek(uint64_t position);
#ifndef WIN32
  int fileno() const { return ::fileno(this->fp); }
#endif
  void close() override;
  void reopen();

  Filesystem::Path getFilename() const { return filename; }

  using WriteStream::write;
protected:
  FileWriteStream(FILE* fp)
    : fp(fp) {}

  void open(const ::Filesystem::Path& path, bool append);

  Filesystem::Path filename;
  FILE* fp;
};
