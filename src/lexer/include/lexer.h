#ifndef __LEXER_H
#define __LEXER_H

#include <vector>
#include <optional>
#include <cstdint>

enum class TokenTypes
{
  OpUnknown,
  // things
  OpExit,
  OpRet,
  OpPrint,
  // keywords
  OpVar,
  // data types
  OpLabelU64,
  OpLabelU32,
  OpLabelU16,
  OpLabelU8,
  OpLabelS64,
  OpLabelS32,
  OpLabelS16,
  OpLabelS8,
  OpLabelVoid,
  // misc
  OpComment,
  OpFunction
};

enum class TokenAttributes
{
  NoAttributes,
  AttributeString,
};

struct Token
{
  TokenTypes Type;
  // the token may require a value, or it may not
  std::optional<std::string>Value;
  // What is special about the token?
  TokenAttributes Attributes;
};

namespace Lexer
{
  int GetTokens(char * line, std::vector<Token>&token_list);
};

#endif
