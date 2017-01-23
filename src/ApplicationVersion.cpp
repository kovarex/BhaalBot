#include <ApplicationVersion.hpp>
#include <StringUtil.hpp>

#define VERSION_STRING "0.1.1"
#define BUILD_VERSION 0

const ApplicationVersion ApplicationVersion::currentVersion(VERSION_STRING, BUILD_VERSION);

ApplicationVersion::ApplicationVersion(const char* versionString,
                                       uint16_t buildVersion)
  : super(versionString)
  , buildVersion(buildVersion)
{}

std::string ApplicationVersion::strDetailed() const
{
  return ssprintf("%s (build %u)", super::str().c_str(), this->buildVersion);
}

bool ApplicationVersion::operator==(const ApplicationVersion& other) const
{
  return super::operator==(other) &&
         buildVersion == other.buildVersion;
}

bool ApplicationVersion::operator!=(const ApplicationVersion& version) const
{
  return !(*this == version);
}
