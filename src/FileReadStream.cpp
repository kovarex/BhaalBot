#include <FileReadStream.hpp>
#include <errno.h>
#include <stdexcept>
#include <string.h>

void FileReadStream::init(const Filesystem::Path& path)
{
  this->filename = path;
#ifdef WIN32
  this->fp = _wfopen(path.wstring().c_str(), L"rb");
#else
  this->fp = fopen(path.string().c_str(), "rb");
#endif
  if (!this->fp)
    throw std::runtime_error("Error when opening " + path.string() + " for reading: " +
                             strerror(errno));
}

FileReadStream::~FileReadStream()
{
  if (this->fp)
    fclose(this->fp);
}

uint32_t FileReadStream::read(char* target, uint32_t size)
{
  uint32_t count = uint32_t(fread(target, 1, size, this->fp));

  if (count != size && ferror(this->fp))
    throw std::runtime_error("Error when reading from file.");

  return count;
}

bool FileReadStream::eof() const
{
  return !!feof(this->fp);
}

uint64_t FileReadStream::ftell()
{
  return ::ftell(this->fp);
}

void FileReadStream::seek(uint64_t position)
{
  if (fseek(this->fp, long(position), SEEK_SET))
    throw std::runtime_error("Seek failed");
}
