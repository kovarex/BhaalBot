#pragma once
#include <WriteStream.hpp>
class StringReadStream;
class ZipPackage;

class StringWriteStream : public WriteStream
{
public:
  StringWriteStream() {}
  virtual ~StringWriteStream() {}
  virtual void write(const char* data, uint32_t size) override
  {
    this->data.append(data, size);
  }
  virtual void flush() override {}
  virtual void close() override {}
  std::string str() const { return data; }
  std::string && moveData()
  {
    return std::move(data);
  }

  using WriteStream::write;

private:
  friend class StringReadStream;
  friend class ZipPackage;
  std::string data;
};
