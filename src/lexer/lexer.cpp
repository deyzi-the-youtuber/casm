#include <iostream>
#include <string.h>
#include <lexer.h>
#include <vector>

static bool line_is_blank(const char * line)
{
  for (const char * c = line; *c; ++c)
  {
    if (!isspace(*c))
      return false;
  }
  return true;
}

static bool operand_is_number(const std::string &op)
{
  for(int i = 0; op[i]; i++)
  {
    if(!std::isdigit(op[i]))
      return false;
  }
  return true;
}

static void line_point(std::string &str)
{
  std::cerr << str << std::endl;
  std::cerr << "! ";
  for(int i = 0; i < str.length() - 2; i++)
  {
    std::cerr << '^';
  }
  std::cerr << " !";
}

int Lexer::GetTokens(char * line, std::vector<Token>&token_list)
{
  static int calls = 0;
  calls++;
  if(line_is_blank(line))
    return 0;
  std::string line_str(line);
  char * token_str;
  token_str = strtok(line, " ");
  while(token_str)
  {
    // where we parse the token string to a value
    if(!strcmp(token_str, "exit"))
    { 
      token_str = strtok(0, " ");
      if(!token_str)
      {
        line_point(line_str);
        std::cout << "\nError: Program exits without an error code!\n";
        return 1;
      }
      if(!operand_is_number(token_str))
      {
        std::cerr << "Operand is not a number!\n";
        return 1;
      }
      token_list.push_back({.Type = TokenTypes::OpExit, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
    } 
    else if(!strcmp(token_str, "return"))
    {
      token_str = strtok(0, " ");
      if(!token_str)
      {
        line_point(line_str);
        std::cerr << "\nWarning: Function returns no value!\n";
      }
      else 
      {
        if(!operand_is_number(token_str))
        {
          line_point(line_str);
          std::cerr << "Operand is not a number!\n";
          return 1;
        }
        token_list.push_back({.Type = TokenTypes::OpRet, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
      }
    }
    // for data types we need to check if we're talking about variables or functions / labels
    else if(!strcmp(token_str, "u64"))
    {
      token_str = strtok(0, " ");
      token_list.push_back({.Type = TokenTypes::OpLabelU64, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
    }
    else if(!strcmp(token_str, "u32"))
    {
      token_str = strtok(0, " ");
      token_list.push_back({.Type = TokenTypes::OpLabelU32, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
    }
    else if(!strcmp(token_str, "u16"))
    {
      token_str = strtok(0, " ");
      token_list.push_back({.Type = TokenTypes::OpLabelU16, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
    }
    else if(!strcmp(token_str, "u8"))
    {
      token_str = strtok(0, " ");
      token_list.push_back({.Type = TokenTypes::OpLabelU8, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
    }
    else if(!strcmp(token_str, "void"))
    {
      token_str = strtok(0, " ");
      token_list.push_back({.Type = TokenTypes::OpLabelVoid, .Value = token_str, .Attributes = TokenAttributes::NoAttributes});
    }
    else if(!strncmp(token_str, "//", 2))
    {
      token_str = strtok(0, " ");
      token_list.push_back({.Type = TokenTypes::OpComment, .Value = line_str, .Attributes = TokenAttributes::NoAttributes});
      return 0;
    }
    else 
    {
      bool we_did_it_hooray = false;
      // check if its a function name 
      for(int i = 0; i < token_list.size(); i++)
      {
        const Token & t = token_list.at(i);
        const char * str = t.Value.value().c_str();
        char * tk_str = token_str;
        strcat(tk_str, ":");
        if(!strcmp(str, tk_str))
        {
          std::string temporary(token_str);
          temporary.erase(strlen(temporary.c_str()) - 1, 1);
          token_list.push_back({.Type = TokenTypes::OpFunction, .Value = temporary, .Attributes = TokenAttributes::NoAttributes});
          we_did_it_hooray = true;
        }
      }
      if(!we_did_it_hooray)
      {
        line_point(line_str);
        std::cerr << "\nInvalid syntax! Line: " << calls << std::endl;
        return 1;
      }
      else 
      {
        token_str = strtok(0, " ");
      }
    }
    token_str = strtok(0, " ");
  }
  return 0;
}
