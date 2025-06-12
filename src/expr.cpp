#include "expr.hpp"
#include "lang.hpp"
#include "tokenize.hpp"
#include "context.hpp"
#include "error.hpp"

#include <vector>
#include <cstring>

static struct literal *parse_num(struct context *ctx, const struct token *tok) {
  struct literal *num = 
    new (ctx->arena->alloc(sizeof(*num))) (struct literal) {
      .type = TYPE_NONE,
    };

  assert(tok->type == INTLIT || tok->type == FLOATLIT);

  if (tok->number.suff.len > 2) {
    EXIT_AND_ERR(ctx->filepath, ctx->src, tok->location, "Invalid suffix %s\n",
                 tok->number.suff.view);
  }

  const char *suff = tok->number.suff.view;

  if (tok->number.suff.len) {
    if (std::strchr(suff, 'l') || std::strchr(suff, 'L')) {
      num->type = TO_LONG(num->type);
    }

    if (std::strchr(suff, 'u') || std::strchr(suff, 'U')) {
      num->type = TO_UNSIGNED(num->type);
    }
  }

  switch (tok->type) {
    case INTLIT: {
      num->type = MERGE_TYPE(num->type, TYPE_INT);
      switch (num->type) {
        case TYPE_INT: case TYPE_SINT: num->lit_int = tok->number.intlit; break;
        case TYPE_UINT: num->lit_uint = tok->number.intlit; break;
        case TYPE_LONG: case TYPE_SLONG: 
          num->lit_long = tok->number.intlit; break;
        case TYPE_ULONG:
          num->lit_ulong = tok->number.intlit; break;

        default:
          EXIT_AND_ERR(ctx->filepath, ctx->src, tok->location, 
                       "Invalid suffix %s\n", tok->number.suff.view);
      }
    } break;

    case FLOATLIT: {
      if (tok->number.suff.len && 
          (std::strchr(suff, 'f') || std::strchr(suff, 'F'))) {
        num->type = MERGE_TYPE(num->type, TYPE_FLOAT);

      } else {
        num->type = MERGE_TYPE(num->type, TYPE_DOUBLE);
      }

      switch (num->type) {
        case TYPE_FLOAT: 
          num->lit_float = std::strtof(tok->number.float_lit.view, NULL);
          break;
        case TYPE_DOUBLE:
          num->lit_double = std::strtod(tok->number.float_lit.view, NULL);
          break;
        case TYPE_LONGDOUBLE:
          num->lit_longdouble = std::strtold(tok->number.float_lit.view, NULL);
          break;

        default:
          EXIT_AND_ERR(ctx->filepath, ctx->src, tok->location, 
                       "Invalid suffix %s\n", tok->number.suff.view);
      }

    } break;

    default: 
      assert(0 && "Unreachable");
  }

  return num;
}

struct expr *parse_expr(struct context *ctx) {
  struct expr *expr = nullptr;

  std::vector<int> op;
  std::vector<struct expr *> output;

  struct token prev = { .type = NONE };
  struct token tok = ctx->lexer->get_tok();
  for (;;) {
    
  }

  ctx->lexer->backtrack(&tok);

  return expr;
}
