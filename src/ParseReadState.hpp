#pragma once
#include <StringUtil.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

class ReadStream;

class ParseReadState
{
public:
  static constexpr int eof = -1;

  ParseReadState(ReadStream* stream, const char* filename = "?");

  int read();

  /** Return a string describing current location in input */
  std::string getLocation() const;

  /** Throw exception complaining about unexpected character */
  [[noreturn]]
  void unexpectedCharacterError(int c) const;

  /// Skips a single character, throws if character is different
  void skip(char c);
  /// skips multiple characters one after another, throws if the sequence is diffrerent
  void skip(const char* string);

  /// Skips zero or more ascii whitespace characters
  void skipWhitespace(bool includingNewline = true);

  uint32_t line = 1;
  uint32_t column = 0;
  std::string filename;
  int lookahead = eof;
private:
  ReadStream* stream;
};

class ParseError : public std::runtime_error
{
public:
  ParseError(const ParseReadState* state, std::string str)
    : ParseError(str, state->filename, state->line) {}

  ParseError(std::string str, std::string file, uint32_t line)
    : std::runtime_error(str + " at " + file + ssprintf(":%d", line))
    , str(str)
    , file(file)
    , line(line) {}

  std::string str;
  std::string file;
  uint32_t line;
};
