#include "parse.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "types.hpp"
#include "expr.hpp"

#include <cstdlib>


struct ast_node parse(class lexer *lexer, class arena *scratch,
                      class arena *arena) {

  struct ast_node root;
  (void)arena;
  (void)scratch;
  struct token tok;
  for (;;) {
    tok = lexer->get_tok();
    switch (tok.type) {
      case END_OF_FILE:
        return root;

      default:
        ;
    }
  }
}
