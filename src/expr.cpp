#include "expr.hpp"
#include "lang.hpp"
#include "tokenize.hpp"
#include "context.hpp"
#include "error.hpp"
#include "parse-types.hpp"

#include <vector>
#include <cstring>
#include <limits.h>
#include <utility>
#include <tuple>

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
    case CHARLIT: 
      switch (tok->charlit) {
        case '#': case '{': case '}': case '\\':
          return false;

        default:
          return true;
      }


    case EQ: case NOTEQ: case LESSEQ: case GREATEREQ: case ANDAND:
    case OROR: case SHL: case SHR: case PLUSPLUS: case MINUSMINUS: case PLUSEQ:
    case MINUSEQ: case MULEQ: case DIVEQ: case MODEQ: case ANDEQ: case OREQ:
    case XOREQ: case ARROW: case EQARROW: case SHLEQ: case SHREQ:
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

    case OP_TERNARY: case OP_TERN: case OP_ARY:
      return 13;

    case OP_ASSIGN: case OP_PLUSEQ: case OP_MINUSEQ: 
    case OP_MULEQ: case OP_DIVEQ: case OP_MODEQ:
    case OP_SHLEQ: case OP_SHREQ: 
    case OP_ANDEQ: case OP_XOREQ: case OP_OREQ:
      return 14;

    case OP_COMMA:
      return 15;

    case OP_STACK_CLEAR:
      return INT_MAX;

    default:
      assert(0 && "Unreachable");
  }
}

static enum operators get_op(const struct token *tok, 
                             const struct token *prev) {
  switch(tok->type) {
    case ID:
      assert(std::strcmp(tok->str.view, "sizeof") == 0);
      return OP_SIZEOF;

    case EQ:
      return OP_EQ;
    case NOTEQ:
      return OP_NOTEQ;
    case LESSEQ:
      return OP_LESSEQ;
    case GREATEREQ:
      return OP_GREATEREQ;
    case ANDAND:
      return OP_ANDAND;
    case OROR:
      return OP_OROR;
    case SHL:
      return OP_SHL;
    case SHR:
      return OP_SHR;

    case PLUSPLUS: {
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_POSTINC;

      } else {
        return OP_PREINC;
      }
    }
    
    case MINUSMINUS: {
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_POSTDEC;

      } else {
        return OP_PREDEC;
      }
    }

    case PLUSEQ:
      return OP_PLUSEQ;
    case MINUSEQ:
      return OP_MINUSEQ;
    case MULEQ:
      return OP_MULEQ;
    case DIVEQ:
      return OP_DIVEQ;
    case MODEQ:
      return OP_MODEQ;
    case ANDEQ:
      return OP_ANDEQ;
    case OREQ:
      return OP_OREQ;
    case XOREQ:
      return OP_XOREQ;
    case ARROW:
      return OP_ARROW;
    case SHLEQ: 
      return OP_SHLEQ;
    case SHREQ:
      return OP_SHREQ;

    case CHARLIT:
      goto Charlit;

    default:
      return OP_NONE;
  }

Charlit:
  switch (tok->charlit) {
    case '+':
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_PLUS;

      } else {
        return OP_UNARY_PLUS;
      }

    case '-':
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_MINUS;

      } else {
        return OP_UNARY_MINUS;
      }

    case '*':
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_MUL;

      } else {
        return OP_DEREF;
      }

    case '/':
      return OP_DIV;
    case '%':
      return OP_MOD;

    case '|':
      return OP_OR;
    case '&':
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_AND;

      } else {
        return OP_REF;
      }
    case '^':
      return OP_XOR;
    case '~':
      return OP_NEGATE;

    case '!':
      return OP_NOT;

    case '[':
      return OP_SUBSCRIPT;
    case ']':
      return OP_CLOSE_SQR;

    case '(':
      if ((!isop(prev) && islit(prev)) || 
          (prev->type == CHARLIT && (prev->charlit == ')' ||
          prev->charlit == ']'))) {
        return OP_FUNCALL;

      } else {
        return OP_OPEN_PAREN;
      }

    case ')':
      return OP_CLOSE_PAREN;

    case ',':
      return OP_COMMA;

    case '<':
      return OP_LESS;
    case '>':
      return OP_GREATER;

    case '=':
      return OP_ASSIGN;

    case '.':
      return OP_DOT;

    case '?':
      return OP_TERN;
    case ':':
      return OP_ARY;

    default:
      return OP_NONE;
  }
}

static enum op_assocs get_assoc(enum operators op) {
  switch (op) {
    case OP_POSTINC: case OP_POSTDEC: 
    case OP_FUNCALL: case OP_SUBSCRIPT:
    case OP_DOT: case OP_ARROW:
      return ASSOC_L;

    case OP_PREINC: case OP_PREDEC:
    case OP_UNARY_PLUS: case OP_UNARY_MINUS:
    case OP_NOT: case OP_NEGATE:
    case OP_CAST: 
    case OP_DEREF: case OP_REF:
    case OP_SIZEOF:
      return ASSOC_R;

    case OP_MUL: case OP_DIV: case OP_MOD:
    case OP_PLUS: case OP_MINUS:
    case OP_SHL: case OP_SHR:
    case OP_LESS: case OP_GREATER: case OP_LESSEQ: case OP_GREATEREQ:
    case OP_EQ: case OP_NOTEQ:
    case OP_AND: case OP_XOR: case OP_OR:
    case OP_ANDAND: case OP_OROR:
      return ASSOC_L;

    case OP_TERNARY: case OP_TERN: case OP_ARY:
    case OP_ASSIGN: 
    case OP_PLUSEQ: case OP_MINUSEQ: 
    case OP_MULEQ: case OP_DIVEQ: case OP_MODEQ:
    case OP_SHLEQ: case OP_SHREQ:
    case OP_ANDEQ: case OP_XOREQ: case OP_OREQ:
      return ASSOC_R;

    case OP_COMMA: 
      return ASSOC_L;

    default:
      assert(0 && "Unreachable");
  }
}

static bool isbinop(enum operators op) {
  switch (op) {
    case OP_UNARY_PLUS: case OP_UNARY_MINUS: case OP_REF: case OP_DEREF:
    case OP_NEGATE: case OP_NOT: case OP_SIZEOF: 
    case OP_PREINC: case OP_POSTINC: case OP_PREDEC: case OP_POSTDEC:
    case OP_SUBSCRIPT: case OP_CLOSE_SQR: 
    case OP_FUNCALL:
    case OP_OPEN_PAREN: case OP_CLOSE_PAREN: 
    case OP_TERN: case OP_ARY: case OP_TERNARY:
    case OP_CAST:
      return false;

    default:
      return true;
  }
}

static bool isunop(enum operators op) {
  switch (op) {
    case OP_UNARY_PLUS: case OP_UNARY_MINUS: case OP_REF: case OP_DEREF:
    case OP_NEGATE: case OP_NOT: case OP_SIZEOF: 
    case OP_PREINC: case OP_POSTINC: case OP_PREDEC: case OP_POSTDEC:
    case OP_SUBSCRIPT: case OP_FUNCALL:
    case OP_CAST:
      return true;

    default:
      return false;
  }
}

static bool isprefix(enum operators op) {
  switch(op) {
    case OP_UNARY_PLUS: case OP_UNARY_MINUS: case OP_REF: case OP_DEREF:
    case OP_NEGATE: case OP_NOT: case OP_SIZEOF: 
    case OP_PREINC: case OP_PREDEC: 
    case OP_CAST:
      return true;

    default:
      return false;
  }
}

static void pop_op_stack(std::vector<struct expr *> &op_stack, 
                         std::vector<struct expr *> &output_stack,
                         enum operators op) {
  while (op_stack.size()) {
    struct expr *op_back_expr = op_stack.back();

    if (op_back_expr->op == OP_OPEN_PAREN || op_back_expr->op == OP_TERN) {
      return;
    }

    if (get_prec(op) < get_prec(op_back_expr->op) || 
        (get_prec(op) == get_prec(op_back_expr->op) && 
        get_assoc(op) == ASSOC_R)) {
      return;
    }

    output_stack.push_back(op_back_expr);
    op_stack.pop_back();
  }
}

static std::pair<struct expr **, uint64_t> parse_args(struct context *ctx) {
  std::vector<struct expr *> args_vec;
  struct token tok = { };
  for (;;) {
    if (tok.type == CHARLIT && tok.charlit == ')') {
      break;
    }

    tok = ctx->lexer->peek();

    args_vec.push_back(parse_expr(ctx, ",)"));
    tok = ctx->lexer->get_tok();
    if (tok.type != CHARLIT || (tok.charlit != ',' && tok.charlit != ')')) {
      EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                   "unexpected token: %d\n", tok.type);
    }
  }

  struct expr **args = new (ctx->arena->alloc(args_vec.size() * sizeof(*args))) 
      struct expr *[args_vec.size()];

  for (uint64_t i = 0; i < args_vec.size(); i++) {
    args[i] = args_vec[i];
  }

  return { args, args_vec.size() };
}

static int64_t get_next_op(const std::vector<struct expr *> &output) {
  for (uint64_t i = 0; i < output.size(); i++) {
    if (output[i]->type == EXPR_OP)
      return i;
  }

  return -1;
}

static bool expected_expr(struct expr *e) {
  if (e->type != EXPR_OP) return true;
  if (e->op == OP_OPEN_PAREN) return true;
  if (isunop(e->op) && isprefix(e->op)) return true;

  return false;
}

static bool expected_op(struct expr *e) {
  if (e->type != EXPR_OP) return false;
  if (e->op == OP_CLOSE_PAREN || e->op == OP_CLOSE_SQR || e->op == OP_TERN ||
      e->op == OP_ARY) {
    return true;
  }

  if (isbinop(e->op)) return true;
  if (isunop(e->op) && !isprefix(e->op)) return true;

  return false;
}

static struct expr *generate_tree(const struct context *ctx, 
                                  std::vector<struct expr *> &output_stack,
                                  const struct token &tok) {
  if (output_stack.empty()) {
    EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                  "Expected expression\n");
  }

  if (output_stack.size() == 1) {
    return output_stack[0];
  }

  while (output_stack.size() > 1) {
    int64_t i = get_next_op(output_stack);

    auto &op_expr = output_stack[i];

    if (isbinop(op_expr->op)) {
      assert(i >= 2);
      
      op_expr->type = EXPR_BINARY;
      op_expr->binary.left = output_stack[i - 2];
      op_expr->binary.right = output_stack[i - 1];

      output_stack.erase(output_stack.begin() + i - 2, 
                         output_stack.begin() + i);

    } else if (isunop(op_expr->op)) {
      assert(i >= 1);

      op_expr->type = EXPR_UNARY;
      op_expr->unary.operand = output_stack[i - 1];

      output_stack.erase(output_stack.begin() + i - 1);

    } else {
      assert(op_expr->op == OP_TERNARY);
      assert(i >= 3);

      op_expr->type = EXPR_TERNARY;
      op_expr->ternary.condition = output_stack[i - 3];
      op_expr->ternary.succ = output_stack[i - 2];
      op_expr->ternary.fail = output_stack[i - 1];

      output_stack.erase(output_stack.begin() + i - 3, 
                         output_stack.begin() + i);
    }
  }

  assert(output_stack.size() == 1);
  return output_stack[0];
}

struct expr *parse_expr(struct context *ctx, const char *term) {
  std::vector<struct expr *> op_stack;
  std::vector<struct expr *> output_stack;

  bool (*expected)(struct expr *) = [](struct expr *) { return true; };

  struct token prev = { .type = NONE };
  struct token tok = ctx->lexer->get_tok();
  for (;;) {
    if (tok.type == CHARLIT && tok.charlit == ';') {
      if (std::strchr(term, ';') == NULL) {
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                    "Unexpected token ';'\n");
      }

      if (expected == expected_expr) {
          EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                      "Unexpected token\n");
      }

      pop_op_stack(op_stack, output_stack, OP_STACK_CLEAR); 
      if (op_stack.size() != 0) {
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                    "Unmatched '(', '[', or unterminated ternary\n");
      }

      break;
    }

    if (isop(&tok)) {
      enum operators op = get_op(&tok, &prev);
      if (op == OP_NONE) {
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                    "unexpected token: %d\n", tok.type);
      }
      
      if (isbinop(op)) {
        // for (enum operators op_back
        //      ;
        //      ((op_stack.size() && (op_back = op_stack.back()) != OP_OPEN_SQR &&
        //        op_back != OP_OPEN_PAREN && op_back != OP_TERN)) &&
        //      (get_prec(op_back) < get_prec(op) || 
        //       (get_prec(op_back) == get_prec(op) && get_assoc(op) == ASSOC_L))
        //      ;
        //      op_stack.pop_back());

        pop_op_stack(op_stack, output_stack, op);

        struct expr *op_expr = 
          new (ctx->arena->alloc(sizeof(*op_expr))) (struct expr) {
            .type = EXPR_OP,
            .op = op,
          };

        if (!expected(op_expr)) {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                         "Unexpected token\n");
        }

        if (op == OP_COMMA && op_stack.empty() && std::strchr(term, ',')) {
          break;
        }

        op_stack.push_back(op_expr);

        expected = expected_expr;

      } else if (isunop(op)) {
Unary:
        struct expr *op_expr = 
          new (ctx->arena->alloc(sizeof(*op_expr))) (struct expr) {
            .type = EXPR_OP,
            .op = OP_NONE,
          };

        if (op == OP_FUNCALL) {
          op_expr->op = OP_FUNCALL;
          op_expr->unary.ctx = 
            new (ctx->arena->alloc(sizeof(*op_expr->unary.ctx))) (struct expr) {
              .type = EXPR_NONE, 
            };

          std::tie(op_expr->unary.ctx->func.args, 
                   op_expr->unary.ctx->func.args_count) = 
            parse_args(ctx);

        } else if (op == OP_SUBSCRIPT) {
          op_expr->op = OP_SUBSCRIPT;
          op_expr->unary.ctx = parse_expr(ctx, "]");
          ctx->lexer->get_tok_and_expect(CHARLIT, ']');

        } else if (op == OP_OPEN_PAREN) {
          op_expr->op = OP_CAST;

          auto [base_type, storage, _] = parse_base_type(ctx);
          if (storage != CLASS_NONE) {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                         "Storage class specified in cast expression");
          }
          op_expr->unary.ctx = 
            new (ctx->arena->alloc(sizeof(*op_expr->unary.ctx))) (struct expr) {
              .type = EXPR_NONE, 
            };

          op_expr->unary.ctx->cast_type = 
            std::get<0>(parse_type_expr(ctx, base_type, nullptr));
          ctx->lexer->get_tok_and_expect(CHARLIT, ')');

        } else {
          op_expr->op = op;
        }

        if (!expected(op_expr)) {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                         "Unexpected token\n");
        }
        
        pop_op_stack(op_stack, output_stack, op_expr->op);
        op_stack.push_back(op_expr);

        if (isprefix(op_expr->op)) {
          expected = expected_expr;

        } else {
          expected = expected_op;
        }

      } else if (op == OP_CLOSE_SQR || op == OP_CLOSE_PAREN || 
                 op == OP_ARY) {
        enum operators opening = OP_NONE;
        switch (op) {
          case OP_CLOSE_PAREN : opening = OP_OPEN_PAREN; break;
          case OP_ARY         : opening = OP_TERN; break;

          case OP_CLOSE_SQR   : opening = OP_NONE; break;

          default:
            assert(0 && "Unreachable");
        }

        while (op_stack.size()) {
          struct expr *op_back_expr = op_stack.back();

          if (op_back_expr->op == OP_OPEN_PAREN ||
              op_back_expr->op == OP_TERN) {
            if (op_back_expr->op != opening) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                          "Mismatched: '%c'\n", tok.charlit);
            }

            break;
          }

          output_stack.push_back(op_back_expr);
          op_stack.pop_back();
        }

        if (op_stack.empty()) {
          if (std::strchr(term, tok.charlit) == NULL) {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                        "Mismatched: '%c'\n", tok.charlit);
          }

          break;
        }

        if (struct expr e = { .type = EXPR_OP, .op = op };
            !expected(&e)) {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                        "Unexpected token\n");
        }

        if (op == OP_ARY) {
          assert(op_stack.back()->op == OP_TERN);
          op_stack.pop_back();

          struct expr *op_ternary = 
            new (ctx->arena->alloc(sizeof(*op_ternary))) (struct expr) {
              .type = EXPR_OP,
              .op = OP_TERNARY,
            };
          op_stack.push_back(op_ternary);

          expected = expected_expr;

        } else {
          op_stack.pop_back();
          expected = expected_op;
        }

      } else {
        // '(', '?'
        assert(op == OP_OPEN_PAREN || op == OP_TERN);

        {
          struct token peek = ctx->lexer->peek();
          if (peek.type == ID && (type_headers.contains(TO_STD_SV(peek.str)) ||
              find_type_in_scope(&peek.str, &ctx->scopes) != nullptr)) {
            goto Unary;
          }
        }

        if (op == OP_TERN) {
          pop_op_stack(op_stack, output_stack, op);
        } 

        struct expr *op_expr = 
          new (ctx->arena->alloc(sizeof(*op_expr))) (struct expr) {
            .type = EXPR_OP,
            .op = op,
          };

        if (!expected(op_expr)) {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                         "Unexpected token\n");
        }

        op_stack.push_back(op_expr);
        expected = expected_expr;
      }

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

          if (prev.type == DQSTRING) {
            output_stack.push_back(
              new (ctx->arena->alloc(sizeof(struct expr))) (struct expr) {
                .type = EXPR_OP,
                .op = OP_CONCAT,
              });
          }
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

      if (!expected(val)) {
          EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                        "Unexpected token\n");
      }

      output_stack.push_back(val);

      if (tok.type == DQSTRING) {
        expected = [](struct expr *e) {
          if (e->type == EXPR_SIMPLE_LIT && e->value->type == TYPE_STR) {
            return true;
          }

          return expected_op(e);
        };

      } else {
        expected = expected_op;
      }

    } else {
      EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                   "unexpected token: %d\n", tok.type);
    }

    prev = tok;
    tok = ctx->lexer->get_tok();
  }

  ctx->lexer->backtrack(&tok);
  assert(op_stack.empty());

  return generate_tree(ctx, output_stack, tok);
}
