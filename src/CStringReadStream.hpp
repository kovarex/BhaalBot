#pragma once
#include <ReadStream.hpp>

class CStringReadStream : public ReadStream
{
public:
  CStringReadStream() = delete;
  CStringReadStream(const char* data)
    : data(data)
  {}

  uint32_t read(char* buffer, uint32_t size) override
  {
    uint32_t copied = 0;
    while (*this->data && copied < size)
    {
      *buffer = *this->data;
      copied++;
      buffer++;
      this->data++;
    }
    return copied;
  }

  bool eof() const override
  {
    return !*this->data;
  }
private:
  const char* data;
};
