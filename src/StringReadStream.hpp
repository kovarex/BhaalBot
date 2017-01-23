#pragma once
#include <ReadStream.hpp>
#include <string>
class StringWriteStream;

class StringReadStream : public ReadStream
{
public:
  StringReadStream() : pos(0) {}
  StringReadStream(const std::string& data)
    : data(data)
    , pos(0)
  {}
  StringReadStream(std::string&& data)
    : data(std::move(data))
    , pos(0)
  {}

  uint32_t read(char* buffer, uint32_t size) override
  {
    uint32_t ret = uint32_t(this->data.copy(buffer, size, this->pos));
    this->pos += ret;
    return ret;
  }

  bool eof() const override
  {
    return this->pos == this->data.size();
  }

  std::string alreadyRead() const
  {
    return this->data.substr(0, this->pos);
  }

  std::string remainder() const
  {
    return this->data.substr(this->pos);
  }

  const std::string& allData() const
  {
    return this->data;
  }

  void stealDataFrom(StringWriteStream* stringWriteStream);

  int64_t remaining() const override { return this->data.size() - this->pos; }
private:
  std::string data;
  uint32_t pos;
};
