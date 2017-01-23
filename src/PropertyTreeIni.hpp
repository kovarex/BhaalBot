#pragma once
#include <PropertyTree.hpp>
#include <ParseReadState.hpp> // For ParseException
#include <Filesystem.hpp>

class WriteStream;

void writeIni(WriteStream& stream, const PropertyTree& ptree);
void writeIni(const Filesystem::Path& path, const PropertyTree& ptree);
std::string writeIniStr(const PropertyTree& ptree);

PropertyTree readIni(ReadStream& stream);
PropertyTree readIni(const Filesystem::Path& path);
PropertyTree readIniStr(const std::string& str);
