#include "expr.hpp"
#include "lang.hpp"
#include "tokenize.hpp"
#include "context.hpp"
#include "error.hpp"

#include <vector>
#include <cstring>

enum op_assocs {
  ASSOC_L,
  ASSOC_R,
};

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
        case TYPE_INT: case TYPE_SINT: 
          num->lit_int = tok->number.intlit; break;
        case TYPE_UINT: 
          num->lit_uint = tok->number.intlit; break;
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

static bool isop(const struct token *tok) {
  if (tok->type == ID && std::strcmp(tok->str.view, "sizeof") == 0) {
    return true;
  }

  switch (tok->type) {
    case CHARLIT: case EQ: case NOTEQ: case LESSEQ: case GREATEREQ: case ANDAND:
    case OROR: case SHL: case SHR: case PLUSPLUS: case MINUSMINUS: case PLUSEQ:
    case MINUSEQ: case MULEQ: case DIVEQ: case MODEQ: case ANDEQ: case OREQ:
    case XOREQ: case ARROW: case EQARROW: case SHLEQ: case SHREQ:
    case POUNDPOUND: case ELIPSES:
      return true;

    default:
      return false;
  }

}

// Must only be called if isop fails
static bool islit(const struct token *tok) {
  switch (tok->type) {
    case INTLIT: case FLOATLIT: case ID: case DQSTRING: case SQSTRING:
      return true;

    default:
      return false;
  }
}

static int get_prec(enum operators op) {
  switch (op) {
    case OP_POSTINC: case OP_POSTDEC: 
    case OP_FUNCALL: case OP_SUBSCRIPT: 
    case OP_DOT: case OP_ARROW: 
      return 1;

    case OP_PREINC: case OP_PREDEC: 
    case OP_UNARY_PLUS: case OP_UNARY_MINUS:
    case OP_NOT: case OP_NEGATE: 
    case OP_CAST: 
    case OP_DEREF: case OP_REF:
    case OP_SIZEOF:
      return 2;

    case OP_MUL: case OP_DIV: case OP_MOD:
      return 3;

    case OP_PLUS: case OP_MINUS:
      return 4;

    case OP_SHL: case OP_SHR:
      return 5;

    case OP_LESS: case OP_GREATER: case OP_LESSEQ: case OP_GREATEREQ:
      return 6;

    case OP_EQ: case OP_NOTEQ:
      return 7;

    case OP_AND:
      return 8;

    case OP_XOR:
      return 9;

    case OP_OR:
      return 10;

    case OP_ANDAND:
      return 11;
      
    case OP_OROR:
      return 12;

    case OP_TERNARY:
      return 13;

    case OP_ASSIGN: case OP_PLUSEQ: case OP_MINUSEQ: 
    case OP_MULEQ: case OP_DIVEQ: case OP_MODEQ:
    case OP_SHLEQ: case OP_SHREQ: 
    case OP_ANDEQ: case OP_XOREQ: case OP_OREQ:
      return 14;

    case OP_COMMA:
      return 15;

    default:
      return -1;
  }
}

struct expr *parse_expr(struct context *ctx) {
  struct expr *expr = nullptr;

  std::vector<int> op;
  std::vector<struct expr *> output;

  struct token prev = { .type = NONE };
  struct token tok = ctx->lexer->get_tok();
  for (;;) {
    if (isop(&tok)) {
    } else if (islit(&tok)) {
      struct expr *val = 
        new (ctx->arena->alloc(sizeof(*val))) struct expr;

      switch (tok.type) {
        case ID:
          val->type = EXPR_SIMPLE_ID;
          val->identifier = tok.str;
          break;

        case FLOATLIT: case INTLIT:
          val->type = EXPR_SIMPLE_LIT;
          val->value = parse_num(ctx, &tok);
          break;

        case DQSTRING:
          val->type = EXPR_SIMPLE_LIT;
          val->value = 
            new (ctx->arena->alloc(sizeof(*val->value))) (struct literal) {
              .type = TYPE_STR,
              .lit_str = tok.str,
            };
          break;

        case SQSTRING:
          val->type = EXPR_SIMPLE_LIT;
          val->value = 
            new (ctx->arena->alloc(sizeof(*val->value))) (struct literal) {
              .type = TYPE_CHAR,
              .lit_char = (assert(tok.str.len == 1), tok.str.view[0]),
            };
          break;

        default:
          assert(0 && "Unreachable");
      }

      output.push_back(val);

    } else {
      EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                   "unexpected token: %d\n", tok.type);
    }

    prev = tok;
    tok = ctx->lexer->get_tok();
  }

  ctx->lexer->backtrack(&tok);

  return expr;
}
