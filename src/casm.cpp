#include <iostream>
#include <fstream>
#include <sstream>
#include <lexer.h>
#include <vector>
#include <sys/stat.h>
#include <cstring>

// making a compiler is ezzzz

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

static std::string Tokens2Asm(const std::vector<Token>&list)
{
  std::stringstream out;
  TokenTypes data_type = TokenTypes::OpUnknown;
  int label_pos;
  out << "section .text\n";
  out << "global _start\n";
  
  for(int i = 0; i < list.size(); i++)
  {
    const Token & t = list.at(i);
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
  std::ifstream fptr(argv[1]);
  for(int i = 0; i < argc; i++)
  {
    if(!strcmp(argv[i], "-o"))
    {
      i++;
      out = argv[i];
      break;
    }
  }
  std::ofstream outfile;
  outfile.open(out);
  // get tokens
  std::string line;
  std::vector<Token>tokens;
  for(; std::getline(fptr, line);)
  {
    char * cstr = line.data();
    if(Lexer::GetTokens(cstr, tokens))
    {
      fptr.close();
      return 1;
    }
  }
  outfile << Tokens2Asm(tokens);
  // now generate the file 
  fptr.close();
  outfile.close();
  return 0;
}
