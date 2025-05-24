#include "parse.hpp"

#include <cstdlib>


struct ast_node parse(class lexer *lexer, class arena *scratch,
                      class arena *arena) {
  struct token tok;
  for (;;) {
    tok = lexer->get_tok();
    switch (tok.type) {
      default:
        ;
    }
  }
}
