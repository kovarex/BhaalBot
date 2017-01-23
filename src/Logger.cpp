#include <ApplicationVersion.hpp>
#include <Log.hpp>
#include <Logger.hpp>
#include <BroodwarWriteStream.hpp>
#include <StringUtil.hpp>
#include <Filesystem.hpp>
#include <FileWriteStream.hpp>
#include <FileUtil.hpp>
#include <chrono>
#include <time.h>
#include <cassert>

Logger* logger = nullptr;

#if defined(LINUX) || defined(MAC_OSX)
#include <execinfo.h>
#include <stdio.h>
#endif
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef MAC_OSX
#include <mach-o/dyld.h>
#endif
#ifdef WIN32
#include <StackWalker.h>
#endif

// This is a static global variable to minimize the include pollution
#ifdef MAC_OSX
_Thread_local char* loggerPrefix;
#else
thread_local char* loggerPrefix;
#endif
bool Logger::skipStacktrace = false;

Logger::Logger(const Filesystem::Path& directory, bool rotate, bool stdoutEnabled)
  : startTime(Logger::microsecondClock())
  , stdoutEnabled(stdoutEnabled)
{
  if (logger != nullptr)
    throw std::runtime_error("Logger already initialized");
  logger = this;
  Filesystem::create_directories(directory);

  Filesystem::Path current = directory / "bhaalbot-current.log";

  rotate = rotate && Filesystem::exists(current);
  if (rotate)
  {
    Filesystem::Path previous = directory / "bhaalbot-previous.log";
    try
    {
      moveFileSafe(current, previous);
    }
    catch (const Filesystem::SystemError&)
    {
      throw std::runtime_error("Could not move file \"factorio-current.log\". Another instance of Factorio is probably already running.");
    }
  }

  this->stream = new FileWriteStream(current, true);

  try
  {
    if (!rotate)
      this->log(__FILE__, __LINE__, LogLevel::Notice, "Not rotating logs.");
  }
  catch (...)
  {
    delete this->stream;
    this->stream = nullptr;
    throw;
  }
}

Logger::~Logger()
{
  if (logger != this)
    LOG_AND_ABORT("Logger not this when destructing");
  logger = nullptr;
  delete this->stream;
}

void Logger::log(const char* file, uint32_t line, LogLevel level,
                 const char* format, ...)
{
  va_list args;
  va_start(args, format);
  std::string message = vssprintf(format, args);
  va_end(args);

  this->writeRecord(this->formatRecord(file, line, level, message));
  if (level >= LogLevel::Notice)
    this->writeRecord(message, &BroodwarWriteStream::instance);
}

void Logger::log(const char* file, uint32_t line, LogLevel level,
                 const std::string& string)
{
  this->writeRecord(this->formatRecord(file, line, level, string));
}

void Logger::logToStdout(const char* file, uint32_t line, LogLevel level,
                         const std::string& string)
{
  // We will log to bw instead
  writeRecord(formatRecord(0, "", file, line, level, string), &BroodwarWriteStream::instance);
}

void Logger::logToStdout(const char* file,
                         uint32_t line,
                         LogLevel level,
                         const char* format,
                         ...)
{
  va_list args;
  va_start(args, format);
  std::string message = vssprintf(format, args);
  va_end(args);

  writeRecord(formatRecord(0, "", file, line, level, message), &BroodwarWriteStream::instance);
}

std::string Logger::formatRecord(const char* file, uint32_t line,
                                 LogLevel level, const std::string& string)
{
  return formatRecord(this->microsecondFromStart(), Logger::getPrefix(), file, line, level, string);
}

std::string Logger::formatRecord(uint64_t time,
                                 const char* prefix,
                                 const char* file,
                                 uint32_t line,
                                 LogLevel level,
                                 const std::string& string)
{
  std::string ret;

  #ifndef DISABLE_LOG_TIME
  uint32_t seconds = uint32_t(time / 1000000);
  uint32_t miliseconds = (time % 1000000) / 1000;

  ret += ssprintf("%4d.%03d ", seconds, miliseconds);
  #endif
  if (prefix)
  {
    ret += prefix;
    ret += " ";
  }
  if (level != LogLevel::Notice)
  {
    ret += levelName(level);
    ret += ssprintf(" %s:%u: ", basename(file), line);
  }
  ret += string;
  return ret;
}

void Logger::writeRecord(const std::string& record)
{
  this->writeRecord(record, this->stream);
}

void Logger::writeRecord(const std::string& record, WriteStream* stream)
{
  stream->write((const char*)record.data(), uint32_t(record.size()));
  stream->write("\n", 1);
  stream->flush();
}

std::string Logger::levelName(LogLevel level)
{
  switch (level)
  {
    case LogLevel::Debug: return "Debug";
    case LogLevel::Verbose: return "Verbose";
    case LogLevel::Info: return "Info";
    case LogLevel::Script: return "Script";
    case LogLevel::Notice: return "Notice";
    case LogLevel::Warning: return "Warning";
    case LogLevel::Error: return "Error";
  }

  assert(false);
  return "";
}

void Logger::writeFailed()
{
  throw std::runtime_error("Writing in log file failed");
}

Filesystem::Path Logger::getLogFilePath()
{
  return this->stream->getFilename();
}

void Logger::reopenLogFile()
{
  assert(this->stream);
  this->log(__FILE__, __LINE__, LogLevel::Notice, "Reopening log file at %s",
            this->stream->getFilename().string().c_str());
  this->stream->reopen();
  this->log(__FILE__, __LINE__, LogLevel::Notice, "Reopened log file at %s",
            this->stream->getFilename().string().c_str());
}

uint64_t Logger::microsecondClock()
{
  std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

const char* Logger::basename(const char* file)
{
  const char* start = file;
  const char* p = file;

  while (true)
  {
    if (!*p)
      return start;

    if (*p == '/' || *p == '\\')
      start = p + 1;

    ++p;
  }
}

#if defined(LINUX) || defined(MAC_OSX)
static Result symbolizeAddresses(const char* executable,
                                 void** addresses, size_t count,
                                 std::vector<std::string>& symbolsOutput)
{
  if (!executable || !*executable)
    return Result(Result::Fail,
                  LocalisedString::literal("(resolving symbol failed: executable name)"));

#ifdef LINUX
  std::string command = ssprintf("addr2line -C -i -f -e %s", executable);
#else
  #ifdef FACTORIO_PACKAGE
  std::string command = ssprintf("atos -o \"%s.dSYM/Contents/Resources/DWARF/factorio\"", executable);
  #else
  std::string command = ssprintf("atos -o \"%s\"", executable);
  #endif
#endif

  for (unsigned i = 0; i < count; ++i)
    if (addresses[i])
      command += ssprintf(" %p", addresses[i]);
    else
      command += " 0x0";

  LOG_DEBUG("Symbolizing addresses: %s", command.c_str());

  FILE* addr2line = popen(command.c_str(), "r");
  if (!addr2line)
    return Result(Result::Fail,
                  LocalisedString::literal("(resolving symbol failed: popen)"));
  std::stringstream stream;
  char buffer[1024];
  while (size_t received = fread(buffer, 1, sizeof(buffer), addr2line))
    stream.write(buffer, received);
  pclose(addr2line);

  if (stream.str().empty())
    return Result(Result::Fail,
                  LocalisedString::literal("(resolving symbol failed: fread)"));

#ifdef LINUX
  std::string symbol;
  while (!std::getline(stream, symbol).eof())
  {
    std::string location;
    std::getline(stream, location);
    symbolsOutput.push_back(symbol + " at " + location);
  }
#else
  std::string symbol;
  while (std::getline(stream, symbol))
    symbolsOutput.push_back(symbol);
#endif
  return Result(Result::Success, LocalisedString());
}
#endif

#ifdef WIN32
class StackWalkerToStream : public StackWalker
{
public:
  StackWalkerToStream(WriteStream& stream, StackTraceInfo* stackTraceInfo)
    : StackWalker()
    , stream(stream)
    , stackTraceInfo(stackTraceInfo) {}

  void inspect(LPCSTR text)
  {
    if (stackTraceInfo)
    {
      // strstr(str1, str2) returns a pointer to the first occurrence of str2 in str1, or a null pointer if str2 is not part of str1.
      // "text" contains dll name and function address (or instruction pointer value?) so we cannot check for exact match, I think.
      if (strstr(text, "OverlayHookD3D") != nullptr)
        stackTraceInfo->containsOverlayHookD3D = true;
      else if (strstr(text, "TestCooperativeLevel") != nullptr)
        stackTraceInfo->containsTestCooperativeLevel = true;
      else if (strstr(text, "al_create_display") != nullptr)
        stackTraceInfo->containsAllegroCreateDisplay = true;
    }
  }

  virtual void OnOutput(LPCSTR szText)
  {
    this->inspect(szText);
    stream.write(szText, uint32_t(strlen(szText)));
  }
  WriteStream& stream;
  StackTraceInfo* stackTraceInfo;
};
#endif // WIN32

void Logger::writeStacktrace(WriteStream* stream, StackTraceInfo* stackTraceInfo)
{
  if (Logger::skipStacktrace)
  {
    stream->WriteStream::write("Logger::writeStacktrace skipped.\n");
    return;
  }
  (void)(stackTraceInfo);
  std::string intro = "Factorio crashed. Generating symbolized stacktrace, please wait ...\n";
  stream->write(intro.data(), uint32_t(intro.size()));
  stream->flush();
#ifdef WIN32
  StackWalkerToStream stackWalker(*stream, stackTraceInfo);
  stackWalker.ShowCallstack();
#else // WIN32

  char executable[1024];
  #ifdef LINUX
  ssize_t pathLength = readlink("/proc/self/exe", executable, sizeof(executable) - 1);
  if (pathLength < 0)
    pathLength = 0;
  executable[pathLength] = '\0';
  #else
  uint32_t bufsize = sizeof(executable);
  if (_NSGetExecutablePath(executable, &bufsize) != 0)
    executable[0] = '\0';
  #endif

  void* array[256];
  size_t size = backtrace(array, sizeof(array) / sizeof(array[0]));

  // Skip the logger functions in the trace
  unsigned skippedFrames = 1;
  void** usedAddresses = array + skippedFrames;
  size_t usedSize = size - skippedFrames;

  std::vector<std::string> symbols;
  Result result = symbolizeAddresses(executable, usedAddresses, usedSize, symbols);
  if (!result.succeeded())
  {
    LOG_WARNING("Couldn't symbolize stacktrace: %s\n",
                result.getMessage().str().c_str());
    symbols.clear();
  }
  else if (symbols.size() != usedSize)
    LOG_WARNING("Symbols.size() == %zu, usedSize == %zu", symbols.size(), usedSize);

  for (unsigned i = 0; i < usedSize; ++i)
  {
    stream->WriteStream::write(ssprintf("#%-2d %p", i, usedAddresses[i]));

    if (symbols.size() > i)
    {
      stream->WriteStream::write(" in ");
      stream->WriteStream::write(symbols[i]);
    }

    stream->WriteStream::write('\n');
  }

  stream->flush();
#endif // WIN32
}

void Logger::logStacktrace(StackTraceInfo* stackTraceInfo)
{
  writeStacktrace(this->stream, stackTraceInfo);
  this->stream->flush();
  this->logStacktraceToStdout();
}

void Logger::flush()
{
  this->stream->flush();
}

void Logger::logStacktraceToStdout()
{
  writeStacktrace(&BroodwarWriteStream::instance, nullptr);
}

uint64_t Logger::microsecondFromStart() const
{
  return Logger::microsecondClock() - this->startTime;
}

const char* Logger::getPrefix()
{
  return loggerPrefix;
}

void Logger::setPrefix(const char* prefix)
{
  loggerPrefix = const_cast<char*>(prefix);
}
