#pragma once

#ifndef PCC_PARSE_TYPES_H
#define PCC_PARSE_TYPES_H

#include "types.hpp"
#include "decls.hpp"
#include "string_view.hpp"

#include <tuple>

std::tuple<struct type_spec *, enum storage_classes, enum qualifiers> 
parse_base_type(struct context *ctx);

void parse_type_expr(struct context *ctx, class arena *arena,
                     struct type_spec *base_type,
                     struct string_view *identifier);

#endif // PCC_PARSE_TYPES_H
