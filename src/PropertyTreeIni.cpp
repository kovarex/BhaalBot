#include <PropertyTreeIni.hpp>
#include <PropertyTree.hpp>
#include <FileReadStream.hpp>
#include <FileWriteStream.hpp>
#include <StringReadStream.hpp>
#include <StringWriteStream.hpp>
#include <StringUtil.hpp>

#include <cfloat>

static void writeItem(WriteStream& stream, const PropertyTree& ptree)
{
  if (ptree.getType() == PropertyTree::Type::None)
    return;

  const auto& key = ptree.getKey();
  stream.write(key.c_str(), uint32_t(key.size()));
  stream.write('=');

  switch (ptree.getType())
  {
    case PropertyTree::Type::Bool:
      stream.write(ptree.get<bool>() ? "true" : "false");
      break;
    case PropertyTree::Type::Number:
    {
      char buffer[2048];
      size_t length = snprintf(buffer, sizeof(buffer), "%.*g", DBL_DIG, ptree.get<double>());
      stream.write(buffer, uint32_t(length));
      break;
    }
    case PropertyTree::Type::String:
    {
      const auto& innerKey = ptree.get<const PropertyTree::StringType&>();
      stream.write(innerKey.c_str(), uint32_t(innerKey.size()));
      break;
    }
    default:
      throw std::runtime_error("Value at property tree path " + ptree.getPath() + " cannot be represented in ini format.");
  }
  stream.write('\n');
}

static void writeSection(WriteStream& stream, const PropertyTree& ptree)
{
  if (ptree.getType() == PropertyTree::Type::None)
    return;

  ptree.requireDict();

  stream.write('[');
  const auto& key = ptree.getKey();
  stream.write(key.c_str(), uint32_t(key.size()));
  stream.write(']');
  stream.write('\n');

  for (const PropertyTree& item: ptree)
    writeItem(stream, item);

  stream.write('\n');
}

static std::string readSectionHeader(ParseReadState& reader)
{
  reader.skip('[');

  std::string buffer;
  while (true)
  {
    char c = char(reader.read());
    switch (c)
    {
      case ']':
        reader.skipWhitespace(false);
        if (reader.lookahead != ParseReadState::eof)
          reader.skip('\n');
        return buffer;
      case '\n':
      case ParseReadState::eof:
        throw ParseError(&reader, "Unterminated section header");
      default:
        buffer += c;
    }
  }
}

static std::string readKey(ParseReadState& reader)
{
  std::string buffer;
  while (true)
  {
    char c = char(reader.read());
    switch (c)
    {
      case '\n':
      case ParseReadState::eof:
        throw ParseError(&reader, "Missing value");
      case '=':
        StringUtil::trim(buffer);
        return buffer;
      default:
        buffer += c;
    }
  }
}

static std::string readUntilEndOfLine(ParseReadState& reader)
{
  std::string buffer;
  while (true)
  {
    char c = char(reader.read());
    switch (c)
    {
      case '\n':
      case ParseReadState::eof:
        StringUtil::trim(buffer);
        return buffer;
      default:
        buffer += c;
    }
  }
}

static PropertyTree readIni(ParseReadState& reader)
{
  std::string buffer;
  PropertyTree ret;

  PropertyTree* currentSection = &ret;

  while (true)
  {
    reader.skipWhitespace();
    switch (reader.lookahead)
    {
      case ParseReadState::eof:
        return ret;
      case '#':
      case ';':
        readUntilEndOfLine(reader); // This function could theoretically do less, but I don't care
        break;
      case '[':
        currentSection = &ret.create(readSectionHeader(reader));
        *currentSection = PropertyTree::dictionary();
        break;
      default:
      {
        PropertyTree& ptree = currentSection->create(readKey(reader));
        ptree = PropertyTree(readUntilEndOfLine(reader), true);
      }
    }
  }

  return ret;
}

void writeIni(WriteStream& stream, const PropertyTree& ptree)
{
  ptree.requireDict();

  for (const PropertyTree& item: ptree)
    if (item.getType() != PropertyTree::Type::Dictionary)
      writeItem(stream, item);

  for (const PropertyTree& item: ptree)
    if (item.getType() == PropertyTree::Type::Dictionary)
      writeSection(stream, item);
}

void writeIni(const Filesystem::Path& path, const PropertyTree& ptree)
{
  FileWriteStream stream(path);
  writeIni(stream, ptree);
}

std::string writeIniStr(const PropertyTree& ptree)
{
  StringWriteStream stream;
  writeIni(stream, ptree);
  return stream.str();
}

PropertyTree readIni(ReadStream& stream)
{
  ParseReadState reader(&stream);
  return readIni(reader);
}

PropertyTree readIni(const Filesystem::Path& path)
{
  FileReadStream stream(path);
  ParseReadState reader(&stream, path.string().c_str());
  return readIni(reader);
}

PropertyTree readIniStr(const std::string& str)
{
  StringReadStream stream(str);
  return readIni(stream);
}
