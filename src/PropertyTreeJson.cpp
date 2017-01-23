#include <PropertyTreeJson.hpp>
#include <PropertyTree.hpp>
#include <WriteStream.hpp>
#include <FileReadStream.hpp>
#include <FileWriteStream.hpp>
#include <StringReadStream.hpp>
#include <StringWriteStream.hpp>
#include <stdio.h>
#include <math.h>
#include <cfloat>

static void writeValue(WriteStream& stream, const PropertyTree& ptree, bool pretty, size_t indent);
static PropertyTree readValue(ParseReadState& reader);

static void writeIndent(WriteStream& stream, size_t size)
{
  for (size_t i = 0; i < 2 * size; ++i)
    stream.write(' ');
}

static void writeNumber(WriteStream& stream, double num)
{
  char buffer[2048];
  size_t length = snprintf(buffer, sizeof(buffer), "%.*g", DBL_DIG, num);
  stream.write(buffer, uint32_t(length));
}

static void writeString(WriteStream& stream, const char* str, size_t len)
{
  assert(str != nullptr);
  stream.write('"');

  const char* nextToWrite = str;

  while (len--)
  {
    // we're using the fact that all bytes that are a part of a multi byte code point
    // are > 128
    unsigned char c = *str;
    if (c == '"' || c == '\\' || c <= 0x1f)
    {
      if (str != nextToWrite)
        stream.write(nextToWrite, str - nextToWrite);

      stream.write('\\');
      switch (c)
      {
        case '"':
        case '\\':
          stream.write(c); break;
        case '\b':
          stream.write('b'); break;
        case '\f':
          stream.write('f'); break;
        case '\n':
          stream.write('n'); break;
        case '\r':
          stream.write('r'); break;
        case '\t':
          stream.write('t'); break;
        default:
        {
          char buffer[6];
          size_t innerLen = snprintf(buffer, sizeof(buffer), "u%04x", unsigned(c));
          stream.write(buffer, uint32_t(innerLen));
        }
      }

      nextToWrite = str + 1;
    }
    ++str;
  }
  if (str != nextToWrite)
    stream.write(nextToWrite, str - nextToWrite);

  stream.write('"');
}

static void writeList(WriteStream& stream, const PropertyTree& ptree, bool pretty, size_t indent)
{
  if (ptree.getType() == PropertyTree::Type::Dictionary)
    stream.write('{');
  else if (ptree.getType() == PropertyTree::Type::List)
    stream.write('[');
  else
    assert(false);

  if (ptree.size() > 0)
  {
    if (pretty)
      stream.write('\n');

    PropertyTree::const_iterator it = ptree.begin();
    while (it != ptree.end())
    {
      if (pretty)
        writeIndent(stream, indent + 1);

      if (ptree.getType() == PropertyTree::Type::Dictionary)
      {
        writeString(stream, it->getKey().c_str(), it->getKey().size());
        stream.write(':');
        if (pretty)
          stream.write(' ');
      }
      else
        assert(it->getKey() == nullptr);

      writeValue(stream, *it, pretty, indent + 1);

      ++it;
      if (it != ptree.end())
        stream.write(',');

      if (pretty)
        stream.write('\n');
    }

    if (pretty)
      writeIndent(stream, indent);
  }

  if (ptree.getType() == PropertyTree::Type::Dictionary)
    stream.write('}');
  else if (ptree.getType() == PropertyTree::Type::List)
    stream.write(']');
}

static void writeValue(WriteStream& stream, const PropertyTree& ptree, bool pretty, size_t indent)
{
  switch (ptree.getType())
  {
    case PropertyTree::Type::None:
      stream.write("null");
      break;
    case PropertyTree::Type::Bool:
      stream.write(ptree.get<bool>() ? "true" : "false");
      break;
    case PropertyTree::Type::Number:
      writeNumber(stream, ptree.get<double>());
      break;
    case PropertyTree::Type::String:
    {
      auto str = ptree.get<const PropertyTree::StringType&>();
      writeString(stream, str.c_str(), str.size());
      break;
    }
    case PropertyTree::Type::List:
    case PropertyTree::Type::Dictionary:
      writeList(stream, ptree, pretty, indent);
      break;
  }
}

uint16_t read4HexDigits(ParseReadState& reader)
{
  uint16_t ret = 0;
  for (int i = 0; i < 4; ++i)
  {
    int c = reader.read();
    if (c >= '0' && c <= '9')
      ret = ret * 16 + (c - '0');
    else if (c >= 'a' && c <= 'f')
      ret = ret * 16 + 10 + (c - 'a');
    else if (c >= 'A' && c <= 'F')
      ret = ret * 16 + 10 + (c - 'A');
    else
      reader.unexpectedCharacterError(c);
  }
  return ret;
}

static void readUnicodeEscape(ParseReadState& reader, std::string& buffer)
{
  uint32_t c = read4HexDigits(reader);

  if (c > 0xd800 && c <= 0xdbff)
  {
    // surrogate pair, we need to read the next escape as well
    reader.skip("\\u");
    uint32_t c2 = read4HexDigits(reader);

    if (c2 < 0xdc00 || c2 > 0xdfff)
      throw ParseError(&reader, "Invalid Utf 16 low surrogate in json string escapes");

    c = (((c & 0x3ff) << 10) | (c2 & 0x3ff)) + 0x010000;
  }
  else if (c >= 0xdc00 && c <= 0xdfff)
    throw ParseError(&reader, "Unexpected Utf 16 low surrogate in json string escapes");

  // Now c contains the unicode codepoint and we just need to encode it to utf8

  if (c < 0x80)
    // plain ascii char, single byte
    buffer += (unsigned char)c;
  else if (c < 0x800)
  {
    // two bytes
    buffer += (unsigned char)(0xc0 | (c >> 6));
    buffer += (unsigned char)(0x80 | (c & 0x3f));
  }
  else if (c < 0x10000)
  {
    // three bytes
    buffer += (unsigned char)(0xe0 | (c >> 12));
    buffer += (unsigned char)(0x80 | ((c >> 6) & 0x3f));
    buffer += (unsigned char)(0x80 | (c & 0x3f));
  }
  else
  {
    // four bytes
    buffer += (unsigned char)(0xf0 | (c >> 18));
    buffer += (unsigned char)(0x80 | ((c >> 12) & 0x3f));
    buffer += (unsigned char)(0x80 | ((c >> 6) & 0x3f));
    buffer += (unsigned char)(0x80 | (c & 0x3f));
  }
}

static std::string readStringRaw(ParseReadState& reader)
{
  // TODO: performance: this could theoretically return ImmutableString borrowed
  // from a parse-wide buffer that can be directly copied into the property tree
  // value or key
  // But it's likely not worth the trouble.

  reader.skip('"');

  std::string buffer;

  while (true)
  {
    char c = char(reader.read());
    if (c == '"')
      return buffer;
    else if (c == '\\')
    {
      char escapedCharacter = char(reader.read());
      switch (escapedCharacter)
      {
        case '"':
        case '\\':
        case '/':
          buffer.push_back(escapedCharacter);
          break;
        case 'b':
          buffer.push_back('\b');
          break;
        case 'f':
          buffer.push_back('\f');
          break;
        case 'n':
          buffer.push_back('\n');
          break;
        case 'r':
          buffer.push_back('\r');
          break;
        case 't':
          buffer.push_back('\t');
          break;
        case 'u':
          readUnicodeEscape(reader, buffer);
          break;
        default:
          throw ParseError(&reader, "Invalid string escape: '" + std::string(1, escapedCharacter) + "'");
      }
    }
    else if (c == ParseReadState::eof)
      throw ParseError(&reader, "Unterminated string");
    else
      buffer.push_back(c);
  }
}

double readRawJsonNumber(ParseReadState& reader, bool exceptions)
{
  bool negative = (reader.lookahead == '-');
  if (negative)
    reader.read();

  if (reader.lookahead == '0')
  {
    reader.read();
    if (reader.lookahead >= '0' && reader.lookahead <= '9')
      if (exceptions)
        reader.unexpectedCharacterError(reader.read());
      else
        return 0;
  }
  else if (reader.lookahead < '0' || reader.lookahead > '9')
    if (exceptions)
      reader.unexpectedCharacterError(reader.read());
    else
      return 0;

  double mantissa = 0;

  while (reader.lookahead >= '0' && reader.lookahead <= '9')
    mantissa = 10 * mantissa + (reader.read() - '0');

  int32_t exponent = 0;
  if (reader.lookahead == '.')
  {
    reader.read();
    if (reader.lookahead < '0' || reader.lookahead > '9')
      if (exceptions)
        reader.unexpectedCharacterError(reader.read());
      else
        return 0;

    while (reader.lookahead >= '0' && reader.lookahead <= '9')
    {
      mantissa = 10 * mantissa + (reader.read() - '0');
      --exponent;
    }
  }

  if (reader.lookahead == 'e' || reader.lookahead == 'E')
  {
    reader.read();

    bool expNegative = false;
    int32_t exp = 0;

    if (reader.lookahead == '+')
      reader.read();
    else if (reader.lookahead == '-')
    {
      expNegative = true;
      reader.read();
    }

    if (reader.lookahead < '0' || reader.lookahead > '9')
      if (exceptions)
        reader.unexpectedCharacterError(reader.read());
      else
        return 0;

    while (reader.lookahead >= '0' && reader.lookahead <= '9')
      exp = 10 * exp + (reader.read() - '0');

    if (expNegative)
      exponent -= exp;
    else
      exponent += exp;
  }

  double val = mantissa * pow(10, exponent);

  if (negative)
    return -val;
  else
    return val;
}

static PropertyTree readDictionary(ParseReadState& reader)
{
  reader.skip('{');
  reader.skipWhitespace();

  PropertyTree ret = PropertyTree::dictionary();

  while (true)
  {
    if (reader.lookahead == '}')
      break;

    std::string key = readStringRaw(reader);
    reader.skipWhitespace();
    reader.skip(':');
    reader.skipWhitespace();
    try {
      ret.create(key) = readValue(reader);
    }
    catch (PropertyTreeException& e)
    {
      throw ParseError(&reader, e.what());
    }
    reader.skipWhitespace();

    if (reader.lookahead != ',')
      break;

    reader.skip(',');
    reader.skipWhitespace();
  }

  reader.skip('}');

  return ret;
}

static PropertyTree readList(ParseReadState& reader)
{
  reader.skip('[');
  reader.skipWhitespace();

  PropertyTree ret = PropertyTree::list();

  while (true)
  {
    if (reader.lookahead == ']')
      break;

    ret.add(readValue(reader));
    reader.skipWhitespace();

    if (reader.lookahead != ',')
      break;

    reader.skip(',');
    reader.skipWhitespace();
  }

  reader.skip(']');

  return ret;
}

static PropertyTree readValue(ParseReadState& reader)
{
  switch (reader.lookahead)
  {
    case '{':
      return readDictionary(reader);
    case '[':
      return readList(reader);
    case '"':
      return PropertyTree(readStringRaw(reader), true /* anyType */);
      // TODO: Loading as type Any is a hack to work around problems with json
      // exported by boost::property_tree. This should change to just plain String
      // once we're reasonably sure that noone has data from before MapVersion(0, 15, 0, 0).
    case 't':
      reader.skip("true");
      return PropertyTree(true);
    case 'f':
      reader.skip("false");
      return PropertyTree(false);
    case 'n':
      reader.skip("null");
      return PropertyTree();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return PropertyTree(readRawJsonNumber(reader));
    default:
      reader.unexpectedCharacterError(reader.read());
  }
}

void writeJson(WriteStream& stream, const PropertyTree& ptree, bool pretty)
{
  writeValue(stream, ptree, pretty, 0);
  if (pretty)
    stream.write('\n');
}

void writeJson(const Filesystem::Path& path, const PropertyTree& ptree, bool pretty)
{
  FileWriteStream stream(path);
  writeJson(stream, ptree, pretty);
}

std::string writeJsonStr(const PropertyTree& ptree, bool pretty)
{
  StringWriteStream stream;
  writeJson(stream, ptree, pretty);
  return stream.str();
}

PropertyTree readJson(ReadStream& stream)
{
  ParseReadState reader(&stream);
  reader.skipWhitespace();
  return readValue(reader);
}

PropertyTree readJson(const Filesystem::Path& path)
{
  FileReadStream stream(path);
  ParseReadState reader(&stream, path.string().c_str());
  reader.skipWhitespace();
  return readValue(reader);
}

PropertyTree readJsonStr(const std::string& str)
{
  StringReadStream stream(str);
  return readJson(stream);
}
