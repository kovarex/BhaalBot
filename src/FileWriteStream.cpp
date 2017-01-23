#include <FileWriteStream.hpp>
#include <errno.h>
#include <stdexcept>
#include <string.h>

FileWriteStream::FileWriteStream(const Filesystem::Path& path, bool append)
{
  this->open(path, append);
}

FileWriteStream::~FileWriteStream()
{
  this->close();
}

void FileWriteStream::open(const Filesystem::Path& path, bool append)
{
  this->filename = path;
#ifdef WIN32
  this->fp = _wfopen(path.wstring().c_str(), append ? L"ab" : L"wb");
#else
  this->fp = fopen(path.string().c_str(), append ? "ab" : "wb");
#endif
  if (!this->fp)
    throw std::runtime_error("Error when opening " + path.string() + " for writing: " +
                             strerror(errno));
}

void FileWriteStream::close()
{
  if (this->fp)
    fclose(this->fp);
  this->fp = nullptr;
}

void FileWriteStream::write(const char* target, uint32_t size)
{
  if (fwrite(target, 1, size, this->fp) != size)
    throw std::runtime_error("Error when writing to file. File: " + this->filename.generic_string());
}

void FileWriteStream::flush()
{
  if (fflush(this->fp))
    throw std::runtime_error("Flush failed: " + std::string(strerror(errno)));
}

void FileWriteStream::seek(uint64_t position)
{
  if (fseek(this->fp, long(position), SEEK_SET))
    throw std::runtime_error("Seek failed");
}

void FileWriteStream::reopen()
{
  this->close();
  this->open(this->filename, true /* append */);
}
