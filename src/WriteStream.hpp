#pragma once
#include <stdint.h>
#include <string>

class WriteStream
{
public:
  virtual ~WriteStream() noexcept(false) {}
  virtual void write(const char* data, uint32_t size) = 0;
  virtual void flush() = 0;
  virtual void close() = 0;

  // convenience functions:
  void write(const std::string& string) { this->write(string.data(), uint32_t(string.size())); }
  void write(char c) { this->write(&c, 1); }
};
