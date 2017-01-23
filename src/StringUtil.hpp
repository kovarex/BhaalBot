#pragma once
#include <string>

std::string ssprintf(const char* format, ...);
std::string vssprintf(const char* format, va_list args);

namespace StringUtil
{
  std::string narrow(const std::wstring& str);
  std::string narrow(const std::string& str);

  /** Trims spaces off the string returning the result. */
  void trim_left(std::string&);
  void trim_right(std::string&);
  void trim(std::string&);
}
