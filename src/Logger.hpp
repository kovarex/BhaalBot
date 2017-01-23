#pragma once
#include <Filesystem.hpp>
#include <string>
class WriteStream;
class FileWriteStream;

enum class LogLevel
{
  /** Detailed info for developers, may spam the console. */
  Debug = 0,
  /** Detailed info for developers, it needs to be switched on in the settings. */
  Verbose = 1,
  /** Regular info for developers.*/
  Info = 2,
  Script = 3,
  /** Something that might be interesting to the user, logged without source file + line info. */
  Notice = 4,
  /** Serious, but not crashing */
  Warning = 5,
  /** Crashy crashy crash */
  Error = 6,
};

#ifdef LINUX // Clang uses different attributes for (u)int64_t - ll vs l
#ifdef printf
#undef printf
#endif

#define FORMAT_ATTR(string, first) __attribute__((format(printf, string, first)))
#else
#define FORMAT_ATTR(string, first)
#endif

struct StackTraceInfo
{
  bool containsOverlayHookD3D = false;
  bool containsTestCooperativeLevel = false;
  bool containsAllegroCreateDisplay = false;
};

class LoggerInterface
{
public:
  virtual ~LoggerInterface() {}
  virtual void log(const char* file, uint32_t line, LogLevel level, const char* format, ...) FORMAT_ATTR(5, 6) = 0;
  virtual void log(const char* file, uint32_t line, LogLevel level, const std::string& string) = 0;
  virtual void logStacktrace(StackTraceInfo* stackTraceInfo) { (void)(stackTraceInfo); }
  virtual void flush() = 0;

  virtual Filesystem::Path getLogFilePath() = 0;
  virtual void reopenLogFile() {}
};

class Logger : public LoggerInterface
{
public:
  /** Open a log file in the given directory.
   * Filename is selected internally.
   * If rotate is false, then already existing file will be used, otherwise
   * existing logs will get rotated and we will start logging into a fresh one.  */
  Logger(const Filesystem::Path& directory, bool rotate = true, bool stdoutEnabled = true);
  ~Logger();

  INLINE_CPP void log(const char* file, uint32_t line, LogLevel level, const char* format, ...) override FORMAT_ATTR(5, 6);
  INLINE_CPP void log(const char* file, uint32_t line, LogLevel level, const std::string& string) override;
  void logStacktrace(StackTraceInfo* stackTraceInfo) override;
  void flush() override;

  /** Log the message only to stdout, but otherwise exactly as log() would. */
  static INLINE_CPP void logToStdout(const char* file, uint32_t line, LogLevel level, const std::string& string);
  static INLINE_CPP void logToStdout(const char* file, uint32_t line, LogLevel level, const char* format, ...) FORMAT_ATTR(4, 5);
  static void logStacktraceToStdout();
  static std::string levelName(LogLevel level);

  Filesystem::Path getLogFilePath() override;
  void reopenLogFile() override;

  uint64_t microsecondFromStart() const;

private:
  static std::string formatRecord(uint64_t time,
                                  const char* prefix,
                                  const char* file,
                                  uint32_t line,
                                  LogLevel level,
                                  const std::string& string);
  std::string formatRecord(const char* file, uint32_t line,
                           LogLevel level, const std::string& string);
  /** Write already formated message to output (file and stdout for now). */
  INLINE_CPP void writeRecord(const std::string& record);
  static INLINE_CPP void writeRecord(const std::string& record, WriteStream* stream);
  static void writeStacktrace(WriteStream* stream, StackTraceInfo* stackTraceInfo);
  void writeFailed();

  static uint64_t microsecondClock();
  static const char* basename(const char* filename);

  FileWriteStream* stream;
  uint64_t startTime;
  bool stdoutEnabled;

  static const char* getPrefix();
  static void setPrefix(const char* prefix);
public:
  static bool skipStacktrace;
};

#undef FORMAT_ATTR

extern Logger* logger;