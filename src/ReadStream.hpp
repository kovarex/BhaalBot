#pragma once
#include <stdint.h>

class ReadStream
{
public:
  virtual ~ReadStream() noexcept(false) {}

  /** Read given ammount of data into a buffer.
   * Returns number of bytes filled, or zero on EOF. */
  virtual uint32_t read(char* buffer, uint32_t size) = 0;
  virtual bool eof() const = 0;

  /** Returns number of bytes remaining to read or -1 if we can't estimate */
  virtual int64_t remaining() const { return -1; }
};
