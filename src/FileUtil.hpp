#pragma once
#include <Filesystem.hpp>
#include <set>

class PackageWriterPath;

std::vector<std::string> listSubdirs(const Filesystem::Path& path);
std::vector<std::string> listFiles(const Filesystem::Path& path,
                                   std::string onlyExtension = std::string());
void copyOptional(const std::string& file,
                  const Filesystem::Path& from,
                  const Filesystem::Path& to,
                  const std::string& destinationFilename = std::string());
void copyFiles(const std::string& wildcard,
               const Filesystem::Path& source,
               const Filesystem::Path& destination);
void recursiveDirectoryCopy(const Filesystem::Path& source,
                            const Filesystem::Path& destination);
void writeToFile(const Filesystem::Path& path, const std::string& content = "", bool append = false);

/** Return path made relative to some base.
 * For example pathRelativeTo("a/b/c/d.txt" "a/b") == "c/d.txt")
 * Returns empty path if the path is not under the given base. */
Filesystem::Path pathRelativeTo(const Filesystem::Path& path,
                                const Filesystem::Path& base);
/** Return if path is under base, or is equal to base. */
bool isSubdir(const Filesystem::Path& path,
              const Filesystem::Path& base);
/** Same as isSubdir but converts paths to their canonical forms before. */
bool isSubdirCanonical(const Filesystem::Path& path,
                       const Filesystem::Path& base);
/** Same as isSubdir but converts paths to their lexically normal forms before checking
 (removes dots, dot-dots and double separators). */
bool isSubdirLexicallyNormal(const Filesystem::Path& path,
                             const Filesystem::Path& base);

/** Path is expected to be below scriptOutputPath. */
bool checkScriptOutputPath(const Filesystem::Path& path,
                           const Filesystem::Path& scriptOutputPath,
                           bool createParents);

void moveFileSafe(const Filesystem::Path& source, const Filesystem::Path& target);

namespace FileUtil
{
  class ReadError : public std::runtime_error
  {
  public:
    ReadError(const char* what)
      : std::runtime_error(what)
    {}
    ReadError(const std::string& what)
      : std::runtime_error(what)
    {}
  };

  /** It tries to bypass the read-only flag when removing the directory */
  void removeAll(const Filesystem::Path& source);
  void remove(const Filesystem::Path& source);
  void removeAtShutdown(const Filesystem::Path& source);
  void removeFilesScheduledForDeletionAtShutdown();
  uint64_t getFileSize(const Filesystem::Path& path);

#ifndef PACKAGER
  Filesystem::Path findSavePath(const std::string& name);
#endif

  extern std::set<Filesystem::Path> filesToDeleteAtShutdown;
}
