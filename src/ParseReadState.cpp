#include <ParseReadState.hpp>
#include <StringUtil.hpp>
#include <cassert>
#include <ReadStream.hpp>

ParseReadState::ParseReadState(ReadStream* stream, const char* filename)
  : filename(filename)
  , stream(stream)
{
  this->read();
}

int ParseReadState::read()
{
  int ret = this->lookahead;
  char c;
  if (this->stream->read(&c, 1))
    this->lookahead = c;
  else
  {
    assert(this->stream->eof());
    this->lookahead = eof;
    return ret;
  }

  if (ret == '\n')
  {
    ++this->line;
    this->column = 0;
  }
  else if (ret != eof)
    ++this->column;

  return ret;
}

void ParseReadState::skip(char c)
{
  int read = this->read();
  if (read != c)
    this->unexpectedCharacterError(read);
}

void ParseReadState::skip(const char* string)
{
  while (*string)
    this->skip(*(string++));
}

void ParseReadState::skipWhitespace(bool includingNewline)
{
  while (true)
  {
    switch (this->lookahead)
    {
      case '\n':
        if (!includingNewline)
          return;
      case ' ':
      case '\r':
      case '\t':
        this->read();
        break;
      default:
        return;
    }
  }
}

void ParseReadState::unexpectedCharacterError(int c) const
{
  if (c != eof)
    throw ParseError(this, "Unexpected character");
  else
    throw ParseError(this, "Unexpected EOF");
}
