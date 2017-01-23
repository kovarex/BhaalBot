#include <cctype>
#include <functional>
#include <StringUtil.hpp>
#include <locale>
#include <codecvt>
#include <stdarg.h>
#include <Log.hpp>

thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>> wstringConvert;

std::string string_vsprintf(const char* format, va_list args)
{
  va_list tmp_args; //unfortunately you cannot consume a va_list twice
  va_copy(tmp_args, args); //so we have to copy it
  const int required_len = vsnprintf(nullptr, 0, format, tmp_args);
  va_end(tmp_args);

  std::string buf;
  buf.resize(required_len + 1);
  if (std::vsnprintf(&buf[0], buf.size(), format, args) < 0)
    LOG_AND_ABORT("string_vsprintf encoding error");
  buf.resize(required_len);
  return buf;
}

std::string ssprintf(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  std::string str(string_vsprintf(format, args));
  va_end(args);
  return str;
}

std::string vssprintf(const char* format, va_list args)
{
  return string_vsprintf(format, args);
}

namespace StringUtil
{

  std::string narrow(const std::wstring& str)
  {
    return wstringConvert.to_bytes(str);
  }

  std::string narrow(const std::string& str)
  {
    return str;
  }

  void trim_left(std::string& s)
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  }

  void trim_right(std::string& s)
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  }

  void trim(std::string& s)
  {
    trim_left(s);
    trim_right(s);
  }
}