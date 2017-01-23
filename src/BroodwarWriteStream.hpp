#pragma once
#include <WriteStream.hpp>

class BroodwarWriteStream : public WriteStream
{
public:
  BroodwarWriteStream() {}
  virtual ~BroodwarWriteStream() {}
  void write(const char* data, uint32_t size) override;
   virtual void flush() override {}
  virtual void close() override {}
  std::string currentLine;

  static BroodwarWriteStream instance;
};
