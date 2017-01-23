#include <BroodwarWriteStream.hpp>
#include <BWAPI.h>

BroodwarWriteStream BroodwarWriteStream::instance;

void BroodwarWriteStream::write(const char* data, uint32_t size)
{
  currentLine += data;
  while (true)
  {
    std::size_t position = currentLine.find('\n');
    if (position == std::string::npos)
      return;
    std::string line = currentLine.substr(0, position);
    BWAPI::Broodwar->printf("%s", line.c_str());
    currentLine = currentLine.substr(position + 1, currentLine.size() - position - 1);
  }
}
