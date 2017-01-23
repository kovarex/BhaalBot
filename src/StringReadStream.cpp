#include <StringReadStream.hpp>
#include <StringWriteStream.hpp>

void StringReadStream::stealDataFrom(StringWriteStream* stringWriteStream)
{
  std::swap(stringWriteStream->data, this->data);
}
