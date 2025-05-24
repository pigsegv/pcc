#include "lexer.hpp"
#define UTIL_FILE_IO_IMPLEMENTATION
#include "file-io.hpp"
#include "arena.hpp"
#include "dynamic-array.hpp"
#include "parse.hpp"

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

  class arena scratch = arena(), mem = arena();

  class lexer lexer(contents, argv[1], &scratch, &mem);
  parse(&lexer, &scratch, &mem);

  // for (struct token tok = lexer.get_tok(); 
      // tok.type != END_OF_FILE;
      // tok = lexer.get_tok()) {
    // printf("--------------------------------\n");
    // switch (tok.type) {
      // case PARSE_ERROR:
        // continue;
      // case DQSTRING: case SQSTRING:
        // printf("str = \"%.*s\"\n", (int)tok.str.len, tok.str.view);
        // break;
      // case CHARLIT:
        // if (tok.charlit == '\n')
          // printf("charlit = \\n\n");
        // else
          // printf("charlit = %c\n", tok.charlit);
        // break;
      // case EQ:
        // printf("charlit = ==\n");
        // break;
      // case NOTEQ:
        // printf("charlit = !=\n");
        // break;
      // case LESSEQ:
        // printf("charlit = <=\n");
        // break;
      // case GREATEREQ:
        // printf("charlit = >=\n");
        // break;
      // case ANDAND:
        // printf("charlit = &&\n");
        // break;
      // case OROR:
        // printf("charlit = ||\n");
        // break;
      // case SHL:
        // printf("charlit = <<\n");
        // break;
      // case SHR:
        // printf("charlit = >>\n");
        // break;
      // case PLUSPLUS:
        // printf("charlit = ++\n");
        // break;
      // case MINUSMINUS:
        // printf("charlit = --\n");
        // break;
      // case PLUSEQ:
        // printf("charlit = +=\n");
        // break;
      // case MINUSEQ:
        // printf("charlit = -=\n");
        // break;
      // case MULEQ:
        // printf("charlit = *=\n");
        // break;
      // case DIVEQ:
        // printf("charlit = /=\n");
        // break;
      // case MODEQ:
        // printf("charlit = %%=\n");
        // break;
      // case ANDEQ:
        // printf("charlit = &=\n");
        // break;
      // case OREQ:
        // printf("charlit = |=\n");
        // break;
      // case XOREQ:
        // printf("charlit = ^=\n");
        // break;
      // case SHLEQ:
        // printf("charlit = <<=\n");
        // break;
      // case SHREQ:
        // printf("charlit = >>=\n");
        // break;
      // case ELIPSES:
        // printf("charlit = ...\n");
        // break;
      // case ARROW:
        // printf("charlit = ->\n");
        // break;
      // case POUNDPOUND:
        // printf("charlit = ##\n");
        // break;
// 
      // case ID:
        // printf("id = %.*s\n", (int)tok.str.len, tok.str.view);
        // break;
// 
      // case INTLIT:
        // printf("intlit = %lu%.*s\n", tok.number.intlit,
                // (int)tok.number.suff.len, tok.number.suff.view);
        // break;
// 
      // case FLOATLIT:
        // printf("floatlit = %.*s%.*s\n", (int)tok.number.float_lit.len,
                // tok.number.float_lit.view, (int)tok.number.suff.len,
                // tok.number.suff.view);
        // break;
       //  
      // default:
        // printf("Unkown\n");
    // }
  // }

  delete[] contents;
  return 0;
}
