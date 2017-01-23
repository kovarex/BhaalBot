#include <Version.hpp>
#include <StringUtil.hpp>

const char* Version::format = "%u.%u.%u";

void Version::setFromString(const char* version)
{
  uint32_t tmpMajor, tmpMinor, tmpSub;
  int32_t scanned = sscanf(version, format,
                           &tmpMajor, &tmpMinor, &tmpSub);
  if (scanned == 2)
  {
    majorVersion = uint16_t(tmpMajor);
    minorVersion = uint16_t(tmpMinor);
    subVersion = 0;
    return;
  }

  if (scanned != 3)
  {
    majorVersion = 0;
    minorVersion = 0;
    subVersion = 0;
  }
  else
  {
    majorVersion = uint16_t(tmpMajor);
    minorVersion = uint16_t(tmpMinor);
    subVersion = uint16_t(tmpSub);
  }
}

std::string Version::str() const
{
  return ssprintf(format, majorVersion, minorVersion, subVersion);
}

bool Version::operator==(const Version& other) const
{
  return majorVersion == other.majorVersion &&
         minorVersion == other.minorVersion &&
         subVersion == other.subVersion;
}

bool Version::operator<(const Version& other) const
{
  if (majorVersion != other.majorVersion)
    return majorVersion < other.majorVersion;
  else if (minorVersion != other.minorVersion)
    return minorVersion < other.minorVersion;
  else
    return subVersion < other.subVersion;
}
