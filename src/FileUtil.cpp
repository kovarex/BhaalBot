#include <Filesystem.hpp>
#include <Log.hpp>
#include <FileUtil.hpp>
#include <FileWriteStream.hpp>
#include <StringUtil.hpp>
#include <deque>
#include <fstream>
#include <regex>
#include <cassert>

template <typename Function>
void forEachFile(const Filesystem::Path& directoryPath,
                 const Function& function) {
  Filesystem::ErrorCode error;
  Filesystem::file_status status = Filesystem::status(directoryPath, error);

  if (error ||
      status.type() == Filesystem::status_error ||
      status.type() == Filesystem::file_not_found ||
      status.type() != Filesystem::directory_file)
    return;

  Filesystem::directory_iterator file(directoryPath, error);

  if (error)
  {
    LOG_ERROR("Couldn't read directory %s: %s", directoryPath.string().c_str(), error.message().c_str());
    return;
  }

  Filesystem::directory_iterator end;
  for (; file != end; ++file)
  {
    error.clear();

    status = Filesystem::status(*file, error);
    if (error ||
        status.type() == Filesystem::status_error ||
        status.type() == Filesystem::file_not_found)
      continue;

    function(*file);
  }
}

template <typename Predicate>
std::vector<std::string> listDirectoryMembers(const Filesystem::Path& path,
                                              const Predicate& memberPredicate)
{
  std::vector<std::string> result;
  forEachFile(path,
              [&] (const Filesystem::directory_entry& entry)
  {
    if (memberPredicate(entry))
      result.push_back(entry.path().filename().string());
  });

  std::sort(result.begin(), result.end());
  return result;
}

std::vector<std::string> listSubdirs(const Filesystem::Path& path)
{
  return listDirectoryMembers(path,
                              [] (const Filesystem::directory_entry& entry) { return entry.status().type() == Filesystem::directory_file; });
}

std::vector<std::string> listFiles(const Filesystem::Path& path,
                                   std::string onlyExtension)
{
  auto hasExtension = [&] (const Filesystem::directory_entry& entry)
                      {
                        return entry.status().type() == Filesystem::regular_file &&
                               (onlyExtension.empty() || entry.path().extension() == onlyExtension);
                      };
  return listDirectoryMembers(path, hasExtension);
}

void copyFiles(const std::string& wildcard,
               const Filesystem::Path& source,
               const Filesystem::Path& destination)
{
  if (!Filesystem::is_directory(source))
    throw std::runtime_error("Source directory " + source.string() +
                             " does not exist or is not a directory.");

  const std::regex my_filter(wildcard);

  
  Filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
  for (Filesystem::directory_iterator iterPath = Filesystem::directory_iterator(source);
       iterPath != Filesystem::directory_iterator(); ++iterPath)
  {
    Filesystem::Path path = iterPath->path();

    // Skip if not a file
    if (!Filesystem::is_regular_file(path))
      continue;

    std::smatch what;
    
    // Skip if no match
    if (!std::regex_match(path.filename().string(), my_filter))
      continue;

    // File matches, store it
    // doesn't work for now
    //(destination / path.filename()).writeFile(iterPath);
  }
}

void recursiveDirectoryCopy(const Filesystem::Path& source,
                            const Filesystem::Path& destination)
{
  // doesn't work for now
  /*
  // Check whether the function call is valid
  if (!Filesystem::is_directory(source))
    throw std::runtime_error("Source directory " + source.string() +
                             " does not exist or is not a directory.");
  // Create the destination directory
  
  //destination.createDirectory();

  // Iterate through the source directory
  for (const Filesystem::path& file:
       boost::make_iterator_range(Filesystem::directory_iterator(source),
                                  Filesystem::directory_iterator()))
  {
    if (Filesystem::is_directory(file))
      recursiveDirectoryCopy(file, destination / file.filename());
    else
      (destination / file.filename()).writeFile(file);
  }*/
}

void copyOptional(const std::string& file,
                  const Filesystem::Path& from,
                  const Filesystem::Path& to,
                  const std::string& destinationFilename)
{
  if (Filesystem::exists(from / file))
    Filesystem::copy_file(from / file,
                          destinationFilename.empty() ? to / file : to / destinationFilename,
                          Filesystem::copy_option::overwrite_if_exists);
}

void writeToFile(const Filesystem::Path& path, const std::string& content, bool append)
{
  FileWriteStream stream(path, append);
  stream.write(content.c_str(), uint32_t(content.size()));
}

Filesystem::Path pathRelativeTo(const Filesystem::Path& path,
                                const Filesystem::Path& base)
{
  Filesystem::Path::iterator baseIt = base.begin();
  Filesystem::Path::iterator pathIt = path.begin();

  while (baseIt != base.end() && pathIt != path.end())
  {
    if (*baseIt != *pathIt)
      return Filesystem::Path();

    ++baseIt;
    ++pathIt;
  }

  if (baseIt == base.end())
  {
    Filesystem::Path ret;
    while (pathIt != path.end())
    {
      ret /= *pathIt;
      ++pathIt;
    }

    assert(ret.string() == "." || base / ret == path);

    return ret;
  }
  else
    return Filesystem::Path();
}

bool isSubdir(const Filesystem::Path& path,
              const Filesystem::Path& base)
{
  Filesystem::Path::iterator baseIt = base.begin();
  Filesystem::Path::iterator pathIt = path.begin();

  while (baseIt != base.end() && pathIt != path.end())
  {
    if (*baseIt != *pathIt)
      return false;

    ++baseIt;
    ++pathIt;
  }

  return baseIt == base.end();
}

bool isSubdirLexicallyNormal(const Filesystem::Path& path,
                             const Filesystem::Path& base)
{
  if (Filesystem::exists(path) && Filesystem::exists(base))
    return isSubdir(path.lexically_normal(), base.lexically_normal());
  else
    return false;
}

bool isSubdirCanonical(const Filesystem::Path& path,
                       const Filesystem::Path& base)
{
  using Filesystem::canonical;
  using Filesystem::exists;
  if (exists(path) && exists(base))
    return isSubdir(canonical(path), canonical(base));
  else
    return false;
}

bool checkScriptOutputPath(const Filesystem::Path& path_,
                           const Filesystem::Path& scriptOutputPath,
                           bool createParents)
{
  Filesystem::Path path = Filesystem::weakly_canonical(path_);
  Filesystem::create_directories(scriptOutputPath);
  if (isSubdir(path, Filesystem::canonical(scriptOutputPath)))
  {
    if (createParents)
      Filesystem::create_directories(path.parent_path());
    else if (!Filesystem::exists(path.parent_path()))
      return false;

    return true;
  }
  return false;
}

void moveFileSafe(const Filesystem::Path& source, const Filesystem::Path& target)
{
  Filesystem::ErrorCode error;
  Filesystem::rename(source, target, error);
  if (!error)
    return;
  if (error != Filesystem::ErrorCodes::cross_device_link)
    throw Filesystem::SystemError(error);

  // When moving a file, be extra-safe by using copy with overwrite. This is to avoid deleting the
  // target in case the copy later fails.

  if (Filesystem::is_regular_file(source))
    Filesystem::copy_file(source, target, Filesystem::copy_option::overwrite_if_exists);
  else
  {
    if (Filesystem::exists(target))
      Filesystem::remove_all(target);
    Filesystem::copy(source, target);
  }

  FileUtil::removeAll(source);
}

namespace FileUtil
{
  std::set<Filesystem::Path> filesToDeleteAtShutdown;
  void removeAll(const Filesystem::Path& source)
  {
    LOG_DEBUG("Recursively removing %s", source.string().c_str());

    Filesystem::ErrorCode error;
    Filesystem::file_status status = Filesystem::status(source, error);

    if (error)
    {
      LOG_ERROR("Can't remove %s: %s", source.string().c_str(), error.message().c_str());
      return;
    }

    if (status.type() == Filesystem::regular_file)
    {
      FileUtil::remove(source);
      return;
    }

    forEachFile(source, [] (const Filesystem::directory_entry& file) { FileUtil::removeAll(file.path()); });
    FileUtil::remove(source);
  }

  void remove(const Filesystem::Path& source)
  {
    LOG_DEBUG("Removing %s", source.string().c_str());

    Filesystem::ErrorCode error;
    Filesystem::remove(source, error);

    if (!error)
      return;

    if (error == Filesystem::ErrorCodes::permission_denied)
    {
      LOG_WARNING("Permission denied trying to remove %s. Will set write permissions and try again",
                  source.string().c_str());

      error.clear();

      // Windows needs write permissions for the file itself, *Nix needs write permission for the parent directory.
#ifdef WIN32
      Filesystem::permissions(source,
                              Filesystem::perms::add_perms | Filesystem::perms::others_write | Filesystem::perms::owner_write,
                              error);
#else
      Filesystem::permissions(source.parent_path(),
                              Filesystem::perms::add_perms | Filesystem::perms::owner_write,
                              error);
#endif

      if (!error)
        Filesystem::remove(source, error);
    }

    if (error)
      LOG_ERROR("Couldn't remove %s: %s", source.string().c_str(), error.message().c_str());
  }

  void removeAtShutdown(const Filesystem::Path& source)
  {
    FileUtil::filesToDeleteAtShutdown.insert(source);
  }

  uint64_t getFileSize(const Filesystem::Path& path)
  {
    try
    {
      return Filesystem::file_size(path);
    }
    catch (Filesystem::filesystem_error&)
    {
      return 0;
    }
  }

  void removeFilesScheduledForDeletionAtShutdown()
  {
    for (const Filesystem::Path& file: FileUtil::filesToDeleteAtShutdown)
      FileUtil::removeAll(file);
  }
}
