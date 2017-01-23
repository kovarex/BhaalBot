#pragma once
#include <Version.hpp>

class ApplicationVersion : public Version
{
  using super = Version;
public:
  ApplicationVersion() : buildVersion(0) {}
  ApplicationVersion(const char* versionString,
                     uint16_t buildVersion);
  std::string strDetailed() const;
  static const ApplicationVersion currentVersion;
  bool operator!=(const ApplicationVersion& version) const;
  bool operator==(const ApplicationVersion& other) const;
  bool isValid() const { return *this != ApplicationVersion(); }

private:
  uint16_t buildVersion;
};
