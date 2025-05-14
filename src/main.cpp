#include "lexer.hpp"
#define UTIL_FILE_IO_IMPLEMENTATION
#include "file-io.hpp"
#include "arena.hpp"
#include "dynamic-array.hpp"

#include <cstdio>
#include <iostream>

int main(int argc, char **argv) {
  // TODO: Use a more robust arguments parser
  if (argc <= 2) {
    std::fprintf(stderr, "Usage: %s INFILE OUTFILE\n", argv[0]);
    return 1;
  }

  uint64_t size;
  char *contents = fio_read_file(argv[1], &size);
  if (contents == NULL) {
    perror("Failed to read file");
    return 1;
  }

  std::cout << contents;

  class pcc::lexer lexer(contents);
  for (struct pcc::token tok = lexer.get_tok(); 
      tok.type != pcc::END_OF_FILE;
      tok = lexer.get_tok()) {
    printf("--------------------------------\n");
    switch (tok.type) {
      case pcc::DQSTRING: case pcc::SQSTRING:
        printf("str = \"%.*s\"\n", (int)tok.str.len, tok.str.view);
        break;
      case pcc::CHARLIT:
        if (tok.charlit == '\n')
          printf("charlit = \\n\n");
        else
          printf("charlit = %c\n", tok.charlit);
        break;
      case pcc::EQ:
        printf("charlit = ==\n");
        break;
      case pcc::NOTEQ:
        printf("charlit = !=\n");
        break;
      case pcc::LESSEQ:
        printf("charlit = <=\n");
        break;
      case pcc::GREATEREQ:
        printf("charlit = >=\n");
        break;
      case pcc::ANDAND:
        printf("charlit = &&\n");
        break;
      case pcc::OROR:
        printf("charlit = ||\n");
        break;
      case pcc::SHL:
        printf("charlit = <<\n");
        break;
      case pcc::SHR:
        printf("charlit = >>\n");
        break;
      case pcc::PLUSPLUS:
        printf("charlit = ++\n");
        break;
      case pcc::MINUSMINUS:
        printf("charlit = --\n");
        break;
      case pcc::PLUSEQ:
        printf("charlit = +=\n");
        break;
      case pcc::MINUSEQ:
        printf("charlit = -=\n");
        break;
      case pcc::MULEQ:
        printf("charlit = *=\n");
        break;
      case pcc::DIVEQ:
        printf("charlit = /=\n");
        break;
      case pcc::MODEQ:
        printf("charlit = %%=\n");
        break;
      case pcc::ANDEQ:
        printf("charlit = &=\n");
        break;
      case pcc::OREQ:
        printf("charlit = |=\n");
        break;
      case pcc::XOREQ:
        printf("charlit = ^=\n");
        break;
      case pcc::SHLEQ:
        printf("charlit = <<=\n");
        break;
      case pcc::SHREQ:
        printf("charlit = >>=\n");
        break;
      case pcc::ELIPSES:
        printf("charlit = ...\n");
        break;
      case pcc::ARROW:
        printf("charlit = ->\n");
        break;
      case pcc::POUNDPOUND:
        printf("charlit = ##\n");
        break;

      case pcc::ID:
        printf("id = %.*s\n", (int)tok.str.len, tok.str.view);
        break;
        
      default:
        printf("Unkown\n");
    }
  }

  delete[] contents;
  return 0;
}
