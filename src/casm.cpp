#include <iostream>
#include <fstream>
#include <sstream>
#include <lexer.h>
#include <vector>
#include <sys/stat.h>
#include <cstring>

#define IS_FUNCTION_TOKEN(t) \
  t.Type == TokenTypes::OpLabelU8 || t.Type == TokenTypes::OpLabelU16 \
  || t.Type == TokenTypes::OpLabelU32 || t.Type == TokenTypes::OpLabelU64 \
  || t.Type == TokenTypes::OpLabelVoid

std::ofstream outfile;
std::ifstream fptr;

// making a compiler is ezzzz

static void die(void)
{
  std::cerr << "Failed to compile!\n";
  outfile.close();
  fptr.close();
  exit(1);
}

static int file_exists(const char * file)
{
  struct stat buf;
  return (stat(file, &buf));
}

static void show_usage()
{
  std::cout << "Cool-Assembly Compiler\n";
  std::cerr << "usage: casm [FILE] [ARGUMENTS]\n";
  exit(1);
  __builtin_unreachable();
}

static void detect_duplicate_functions(const std::vector<Token>&list)
{
  std::vector<Token>functions;
  for(int i = 0; i < list.size(); i++)
  {
    const Token &t = list.at(i);
    if(IS_FUNCTION_TOKEN(t))
    {
      functions.push_back(t);
    }
  }
  for(int i = 1; i < functions.size(); i++)
  {
    std::string func = functions.at(i).Value.value();
    std::string func2 = functions.at(i - 1).Value.value();
    if(!func.compare(func2))
    {
      std::cout << "Error: Detected function duplicates! (" << func2 << ")\n";
      die();
    }
  }
}

static std::string Tokens2Asm(const std::vector<Token>&list)
{
  std::stringstream out;
  TokenTypes data_type = TokenTypes::OpUnknown;
  int label_pos;
  out << "; Compiled by CASM\n";
  out << "global _start\n";
  
  for(int i = 0; i < list.size(); i++)
  {
    const Token &t = list.at(i);
    if(t.Type == TokenTypes::OpExit)
    {
      out << "  mov rax, 60\n";
      out << "  mov rdi, " << t.Value.value() << std::endl;
      out << "  syscall\n";
    }
    else if(t.Type == TokenTypes::OpRet)
    {
      if(label_pos < i && data_type == TokenTypes::OpLabelVoid)
      {
        std::cerr << "Warning: Returing a value in a void function!\n";
      }
      if(data_type == TokenTypes::OpLabelU32)
      {
        out << "  mov eax, " << t.Value.value() << std::endl;
      }
      else if(data_type == TokenTypes::OpLabelU64)
      {
        out << "  mov rax, " << t.Value.value() << std::endl;
      }
      out << "  ret\n";
    }
    else if(t.Type == TokenTypes::OpLabelVoid)
    {
      label_pos = i;
      out << t.Value.value() << std::endl;
      data_type = TokenTypes::OpLabelVoid;
    }
    else if(t.Type == TokenTypes::OpLabelU32)
    {
      label_pos = i;
      out << t.Value.value() << std::endl;
      data_type = TokenTypes::OpLabelU32;
    }
    else if(t.Type == TokenTypes::OpLabelU64)
    {
      label_pos = i;
      out << t.Value.value() << std::endl;
      data_type = TokenTypes::OpLabelU64;
    }
    else if(t.Type == TokenTypes::OpFunction)
    {
      out << "  call " << t.Value.value() << std::endl;
    }
  }
  return out.str();
}

int main(int argc, char * argv[])
{
  if(argc < 2)
    show_usage();
  if(file_exists(argv[1]))
  {
    std::cerr << argv[1] << ": No such file or directory\n";
    return 1;
  }
  char * out = (char *)"out.S";
  fptr.open(argv[1]);
  for(int i = 0; i < argc; i++)
  {
    if(!strcmp(argv[i], "-o"))
    {
      i++;
      out = argv[i];
      break;
    }
  }
  outfile.open(out);
  // get tokens
  std::string line;
  std::vector<Token>tokens;
  for(; std::getline(fptr, line);)
  {
    char * cstr = line.data();
    if(Lexer::GetTokens(cstr, tokens))
    {
      outfile.close();
      fptr.close();
      return 1;
    }
  }
  detect_duplicate_functions(tokens);
  outfile << Tokens2Asm(tokens);
  // now generate the file 
  fptr.close();
  outfile.close();
  return 0;
}
