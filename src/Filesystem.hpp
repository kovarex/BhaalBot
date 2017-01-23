#pragma once
#include <filesystem>
#include <system_error>
#include <string>
#include <ctime>

namespace Filesystem = std::experimental::filesystem;

namespace std
{
  namespace experimental
  {
    namespace filesystem
    {
      using ErrorCodes = std::errc;
      using ErrorCode = std::error_code;
      using SystemError = std::system_error;
      using FileTimeType = std::experimental::filesystem::file_time_type;
      using FileType = std::experimental::filesystem::file_type;

      constexpr FileType directory_file = FileType::directory;
      constexpr FileType regular_file = FileType::regular;
      constexpr FileType file_not_found = FileType::not_found;
      constexpr FileType status_error = FileType::none;

      struct copy_option
      {
        static constexpr copy_options overwrite_if_exists = copy_options::overwrite_existing;
      };

      static path weakly_canonical(const path& pval) { return std::experimental::filesystem::canonical(pval); }

      path unique_path(const path& model);
      path SystemComplete(const path& p);
      std::time_t LastWriteTime(const path& p);

      struct Path : public path
      {
        Path() : path() {}
        Path(const char* str) : path(u8path(str)) {}
        Path(const std::string& str) : path(u8path(str)) {}
        Path(const std::wstring& str) : path(str) {}
        Path(path&& p) : path(std::move(p)) {}
        Path(const path& p) : path(p) {}

        std::string string() const;
        std::string generic_string() const { return this->empty() ? std::string() : path::generic_u8string(); }

        Path& normalize() { *this = canonical(*this); return *this; }
        Path lexically_normal() const { return canonical(*this); }
      };

      static Path operator/(const Path& left, const Path& right) { return (*static_cast<const path*>(&left)) / (*static_cast<const path*>(&right)); }
      static Path operator/(const path& left, const Path& right) { return left / (*static_cast<const path*>(&right)); }
      static Path operator/(const Path& left, const path& right) { return (*static_cast<const path*>(&left)) / right; }
      static Path operator/(const Path& left, const char* right) { return left / Path(right); }
      static Path operator/(const Path& left, const std::string& right) { return left / Path(right); }
      static Path operator/(const std::string& left, const Path& right) { return Path(left) / right; }
      static Path operator/(const Path& left, const std::wstring& right) { return left / Path(right); }

      static Path operator/(const path& left, const std::string& right) { return left / Path(right); }
      static Path operator/(const path& left, const std::wstring& right) { return left / Path(right); }
      static Path operator/(const path& left, const char* right) { return left / Path(right); }

      namespace ErrorCategories
      {
        static const std::error_category& SystemCategory() { return std::system_category(); }
      }
    }
  }
}
