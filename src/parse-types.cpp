#include "parse-types.hpp"
#include "types.hpp"
#include "string_view.hpp"
#include "context.hpp"

#include <signal.h>
#include <unistd.h>
#include <tuple>

std::tuple<struct type_spec *, enum storage_classes, enum qualifiers> 
parse_base_type(struct context *ctx) {
  struct type_spec *type = nullptr; 
  enum storage_classes storage = CLASS_NONE;
  enum qualifiers qual = QUAL_NONE;

  struct token tok = ctx->lexer->peek();
  if (tok.type != ID) {
    EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                 "unexpected token %d\n", tok.type);
  }
  
  // get types, qualifiers and storage classes
  if (auto s = keywords.find(TO_STD_SV(tok.str));
      s != keywords.end()) {
    
    struct type_spec tmp = {
      .type = TYPE_NONE,
    };

    for (; tok.type == ID && s != keywords.end(); 
         tok = ctx->lexer->peek(), 
         s = tok.type == ID 
           ? keywords.find(TO_STD_SV(tok.str)) : keywords.end()) {
      ctx->lexer->get_tok();
      switch (s->second) {
        case SIGNED_FLAG:
          tmp.type = TO_SIGNED(tmp.type);
          break;

        case UNSIGNED_FLAG:
          tmp.type = TO_UNSIGNED(tmp.type);
          break;

        case LONG_FLAG:  
          tmp.type = TO_LONG(tmp.type);
          break;

        case SHORT_FLAG:  
          tmp.type = TO_SHORT(tmp.type);
          break;

        case TYPE_NONE: {
          if (std::strcmp(tok.str.view, "static") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }

            storage = CLASS_STATIC;

          } else if (std::strcmp(tok.str.view, "auto") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }
            storage = CLASS_AUTO;

          } else if (std::strcmp(tok.str.view, "register") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }
            storage = CLASS_REGISTER;

          } else if (std::strcmp(tok.str.view, "extern") == 0) {
            if (storage != CLASS_NONE) {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                           "declaration has multiple storage classes\n");
            }
            storage = CLASS_EXTERN;

          }  else if (std::strcmp(tok.str.view, "volatile") == 0) {
            qual = (enum qualifiers)(qual | QUAL_VOLATILE);

          } else if (std::strcmp(tok.str.view, "const") == 0) {
            qual = (enum qualifiers)(qual | QUAL_CONST);

          } else {
            report_error(ctx->filepath, ctx->src, tok.location,
                        "Unexpected token \"%.*s\"\n", 
                        (int)tok.str.len, tok.str.view);
            std::exit(EXIT_FAILURE);
          }
        } break;
          
        default:
          tmp.type = MERGE_TYPE(tmp.type, s->second);
      }
    }
  
    if ((tmp.type & 0x0000ffff) == 0) {
      tmp.type = MERGE_TYPE(tmp.type, TYPE_INT);
    }

    switch (tmp.type) {
      case TYPE_CHAR: case TYPE_INT:   case TYPE_FLOAT: case TYPE_DOUBLE:
      case TYPE_VOID: case TYPE_SCHAR: case TYPE_SINT:  case TYPE_UCHAR:
      case TYPE_UINT: case TYPE_SHORT: case TYPE_LONG:  case TYPE_LONGLONG:
      case TYPE_LONGDOUBLE: case TYPE_SSHORT: case TYPE_SLONG:
      case TYPE_SLONGLONG:  case TYPE_USHORT: case TYPE_ULONG:
      case TYPE_ULONGLONG:  case TYPE_NONE:
        break;

      default:
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                     "declaration has an invalid type: %u\n", tmp.type);
    }

    if (tmp.type != TYPE_NONE) {
      type = new (ctx->scratch.alloc(sizeof(*type))) (struct type_spec) {
        .type = tmp.type,
      };
    }
  } 

  if (type == nullptr) {
    type = find_type_in_scope(&tok.str, &ctx->scopes);
  }

  assert(type != nullptr);

  return { type, storage, qual };
}

// Consumes all tokens until the end of the pointer declaration
static uint32_t get_ptr_level(struct context *ctx) {
  uint32_t level = 0;
  for (;;) {
    auto tok = ctx->lexer->peek();
    
    if (tok.type == CHARLIT) {
      switch (tok.charlit) {
        case '*': break;
        case ';': case '[': case ']': case '(': case ')':
          return level;

        default:
          EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                      "unexpected token: %c\n", tok.charlit);
      }

    } else {
          return level;
    }

    level++;
    ctx->lexer->get_tok();
  }

  return level;
}

struct function parse_func_args(struct context *ctx) {
  struct token tok;

  struct function func = { };

  std::vector<
    std::pair<struct type_spec *, std::optional<struct string_view>>> args;

  for (;;) {
    tok = ctx->lexer->get_tok();

    if (tok.type == ELIPSES) {
      ctx->lexer->get_tok_and_expect(CHARLIT, ')');
      struct type_spec *type = 
        new (ctx->arena->alloc(sizeof(*type))) (struct type_spec) {
          .type = TYPE_VARIADIC,
        };

      args.emplace_back(type, std::nullopt);
      break;
    }

    ctx->lexer->backtrack(&tok);
    auto [base_type, storage, _] = parse_base_type(ctx);

    if (storage != CLASS_NONE) {
      EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                   "Storage class specified for function argument");
    }

    std::optional<struct string_view> ident = std::nullopt;
    struct type_spec *type = 
      std::get<0>(parse_type_expr(ctx, base_type, &ident));

    args.emplace_back(type, ident);

    tok = ctx->lexer->get_tok();
    if (tok.type != CHARLIT || (tok.charlit != ')' && tok.charlit != ',')) {
      EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                   "Expected ',' or ')', got token type: %d\n", tok.type);
    }

    if (tok.charlit == ')') {
      ctx->lexer->backtrack(&tok);
      break;
    }
  }

  func.num_args = args.size();
  if (args.back().first->type == TYPE_VARIADIC) {
    func.variadic = true;
    func.num_args--;
  }

  func.args = 
    new (ctx->arena->alloc(func.num_args * sizeof(*func.args))) 
      struct type_spec *[func.num_args];

  func.arg_names = 
    new (ctx->arena->alloc(func.num_args * sizeof(*func.arg_names))) 
      struct string_view[func.num_args];


  for (uint64_t i = 0; i < func.num_args; i++) {
    func.args[i] = args[i].first;
    assert(func.args[i]->type != TYPE_VARIADIC);

    struct string_view tmp = { .view = nullptr, .len = 0};
    
    if (args[i].second) tmp = args[i].second.value();
    func.arg_names[i] = tmp;
  }


  return func;
}

static void assign_type(struct type_spec *to, struct type_spec *what) {
  switch (to->type) {
    case TYPE_PTR:
      to->ptr.type = what;
      break;
    case TYPE_ARRAY:
      to->array.type = what;
      break;

    case TYPE_FUNC:
      to->func.ret = what;
      break;

    default:
      assert(0 && "unreachable");
  }
}

std::tuple<struct type_spec *, struct type_spec *>
parse_type_expr(struct context *ctx,
                struct type_spec *base_type,
                std::optional<struct string_view> *identifier) {
  struct token tok; 

  struct type_spec *ptr = nullptr;

  tok = ctx->lexer->peek();
  if (tok.type == CHARLIT) {
    switch (tok.charlit) {
      case '*': 
        ptr = new (ctx->arena->alloc(sizeof(*ptr))) (struct type_spec) {
          .type = TYPE_PTR,
          .ptr = {
            .level = get_ptr_level(ctx),
          },
        };
        break;

      case '(':  // This will be handled in the next step
        break;

      case ';': case ')':
        return { base_type, base_type }; // returns nullptr on recursive calls

      default:
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                     "unexpected token: %c\n", tok.charlit);
    }
  }

  struct type_spec *body = nullptr;
  struct type_spec *body_end = nullptr;
  tok = ctx->lexer->peek();
  if (tok.type == CHARLIT) {
    switch (tok.charlit) {
      case '(': {
        ctx->lexer->get_tok();
        std::tie(body, body_end) = parse_type_expr(ctx, nullptr, identifier);
  
        // Function type, or expression termination
        if (body == nullptr) {
          if (ctx->lexer->peek().type == CHARLIT) {
            if (ctx->lexer->peek().charlit == ')') {
              ctx->lexer->backtrack(&tok);

            } else if (ctx->lexer->peek().charlit == ';') {
              return { base_type, base_type };

            } else {
              EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                          "unexpected token: %c\n", ctx->lexer->peek().charlit);
            }
          } else if (ctx->lexer->peek().type == ID || 
                     ctx->lexer->peek().type == ELIPSES) {
              ctx->lexer->backtrack(&tok);

          } else {
            EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                         "unexpected token");
          }

          goto Post;
        }

        ctx->lexer->get_tok_and_expect(CHARLIT, ')');
      } break;
    }

  } else if (tok.type == ID) {
    if (keywords.contains(TO_STD_SV(tok.str)) ||
        find_type_in_scope(&tok.str, &ctx->scopes) != nullptr) {
      if (ptr != nullptr) {
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                    "unexpected symbol: %s\n", tok.str.view);
      }

      return { nullptr, nullptr };
    }

    if (identifier != nullptr) {
      if (*identifier) {
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                    "unexpected symbol: %s\n", tok.str.view);
      }

      *identifier = tok.str;
    }
    
    ctx->lexer->get_tok();

  } else if (tok.type == ELIPSES) {
    return { nullptr, nullptr };

  } else {
    EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                 "unexpected token: %d\n", tok.type);
  }

Post:
  auto arena_save = ctx->arena->save();
  struct type_spec *post = new (ctx->arena->alloc(sizeof(*post))) 
                             (struct type_spec) {
                               .type = TYPE_NONE,
                             };
  
  struct type_spec *prev_post_end = nullptr;
  struct type_spec *post_end = post;
  

  struct type_spec *type = nullptr;
  struct type_spec *type_end = nullptr;

  for (;;) {
    tok = ctx->lexer->get_tok();
    if (tok.type != CHARLIT) {
      EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                  "unexpected token: %d\n", tok.type);
    }

    if (tok.charlit == ';' || tok.charlit == ',' || tok.charlit == ')' || 
        tok.charlit == '=') {
      ctx->lexer->backtrack(&tok);
      break;
    }

    switch (tok.charlit) {
      case '(': {
        *post_end = (struct type_spec) {
          .type = TYPE_FUNC,
          .func = { },
        };

        if (ctx->lexer->peek().type != CHARLIT || 
            ctx->lexer->peek().charlit != ')') {
          post_end->func = parse_func_args(ctx);
        }

        ctx->lexer->get_tok_and_expect(CHARLIT, ')');

        prev_post_end = post_end;

        arena_save = ctx->arena->save();
        post_end->func.ret = 
          new (ctx->arena->alloc(sizeof(*post_end->func.ret))) 
            (struct type_spec) {
              .type = TYPE_NONE,
            };

        post_end = post_end->func.ret;

      } break;

      case '[':
        assert(0 && "TODO: Implement support for arrays");

      case ';':
        break;

      default:
        EXIT_AND_ERR(ctx->filepath, ctx->src, tok.location, 
                    "unexpected token: %c\n", tok.charlit);
    }
  }

  if (post_end->type == TYPE_NONE) {
    ctx->arena->restore(arena_save);
  }
      
  if (ptr) {
    type = ptr;
    type_end = ptr;

    assign_type(type_end, base_type);
  }

  if (prev_post_end) {
    if (type == nullptr) {
      type = post;
      type_end = prev_post_end;

      assign_type(type_end, base_type);


    } else {
      assign_type(prev_post_end, type);
      type = post;
    }
  }

  if (body) {
    if (type == nullptr) {
      type = body;
      type_end = body_end;

      assign_type(type_end, base_type);

    } else {
      assign_type(body_end, type);
      type = body;
    }
  }

  if (type == nullptr) {
    type = base_type;
    type_end = base_type;
  }
  

  return { type, type_end };
}
