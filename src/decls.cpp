#include "decls.hpp"
#include "context.hpp"
#include "lexer.hpp"
#include "expr.hpp"
#include "parse-types.hpp"

#include <cstring>
#include <cstdlib>
#include <vector>
#include <assert.h>
#include <cstdlib>
#include <tuple>
#include <optional>

static struct type_spec *parse_var(struct context *ctx,
                                  struct ast_node *curr) {
  (void)curr;

  auto scratch_save = ctx->scratch.save();
  auto [type, storage, quals] = parse_base_type(ctx);

  std::optional<struct string_view> ident;
  return std::get<0>(parse_type_expr(ctx, type, &ident));
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
    assert(0 && "TODO: Handle record types");

  } else if (std::strcmp(tok.str.view, "typedef") == 0) {
    assert(0 && "TODO: Handle typedefs");
  }

  /* 
   * Possible declarations ->
   *   variable
   *   function
   */
  struct type_spec *t = parse_var(ctx, curr);
  (void)t;
}
