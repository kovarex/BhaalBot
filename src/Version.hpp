#pragma once
#include <stdint.h>
#include <string>
class Deserialiser;
class Serialiser;
class MapDeserialiser;
class PropertyTree;

class Version
{
public:
  /** Construct invalid version */
  Version()
    : majorVersion(0)
    , minorVersion(0)
    , subVersion(0) {}

  Version(uint16_t majorVersion,
          uint16_t minorVersion,
          uint16_t subVersion)
    : majorVersion(majorVersion)
    , minorVersion(minorVersion)
    , subVersion(subVersion) {}
  Version(const PropertyTree& ptree);

  /** Construct version from string in format %u.%u.%u.
   * If the string doesn't match this format, invalid version is constructed. */
  Version(const char* version) { setFromString(version); }
  Version(const std::string& version)  { setFromString(version.c_str()); }

  Version(Deserialiser& input);
  Version(MapDeserialiser& input);
  void save(Serialiser& output) const;
  void save(PropertyTree& ptree,
            std::string key = "") const;

  std::string str() const;

  operator bool() const { return this->isValid(); }
  bool operator==(const Version& other) const;
  bool operator!=(const Version& other) const { return !(*this == other); }
  bool operator<(const Version& other) const;
  bool operator>(const Version& other) const { return other < *this; }
  bool operator<=(const Version& other) const { return *this < other || *this == other; }
  bool operator>=(const Version& other) const { return other < *this || *this == other; }
  uint16_t getMajorVersion() const { return this->majorVersion; }
  uint16_t getMinorVersion() const { return this->minorVersion; }
  uint16_t getSubVersion() const { return this->subVersion; }

  bool isValid() const { return *this != Version(); }
protected:
  void setFromString(const char* version);

  uint16_t majorVersion;
  uint16_t minorVersion;
  uint16_t subVersion;

  static const char* format;
};
