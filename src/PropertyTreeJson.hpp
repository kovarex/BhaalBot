#pragma once
#include <PropertyTree.hpp>
#include <ParseReadState.hpp> // For ParseError
#include <Filesystem.hpp>

class WriteStream;

void writeJson(WriteStream& stream, const PropertyTree& ptree, bool pretty = true);
void writeJson(const Filesystem::Path& path, const PropertyTree& ptree, bool pretty = true);
std::string writeJsonStr(const PropertyTree& ptree, bool pretty = true);

PropertyTree readJson(ReadStream& stream);
PropertyTree readJson(const Filesystem::Path& path);
PropertyTree readJsonStr(const std::string& str);

double readRawJsonNumber(ParseReadState& readState, bool exceptions = true);
