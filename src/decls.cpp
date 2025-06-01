#include "decls.hpp"
#include "context.hpp"

#include <cstring>
#include <cstdlib>
#include <vector>
#include <assert.h>


static struct type_spec parse_var(struct context *ctx,
                                  struct ast_node *curr) {
  struct type_spec type = {
    .type = TYPE_NONE,
  };

  struct token tok = ctx->lexer->get_tok();
  if (auto s = primitives.find(TO_STD_SV(tok.str));
      s != primitives.end()) {
    bool contains = true;
    while (contains) {

      tok = ctx->lexer->get_tok();
      s = primitives.find(TO_STD_SV(tok.str));
      contains = s != primitives.end();
    }
  }
}

void parse_decl(struct context *ctx, struct ast_node *curr) {
  struct token tok = ctx->lexer->peek();

  if (std::strcmp(tok.str.view, "struct") == 0 || 
      std::strcmp(tok.str.view, "union") == 0 || 
      std::strcmp(tok.str.view, "enum") == 0) {
    /* 
     * Possible declarations ->
     *   variable
     *   function
     *   record
     *   enum
     */
    assert(0 && "TODO: Handle non-primitive types");

  } else if (std::strcmp(tok.str.view, "typedef")) {
    assert(0 && "TODO: Handle typedefs");
  }

  /* 
   * Possible declarations ->
   *   variable
   *   function
   */
  struct type_spec t = parse_var(ctx, curr);
}
