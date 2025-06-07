#pragma once

#ifndef PCC_PARSE_TYPES_H
#define PCC_PARSE_TYPES_H

#include "types.hpp"
#include "decls.hpp"
#include "string_view.hpp"

#include <tuple>
#include <optional>

std::tuple<struct type_spec *, enum storage_classes, enum qualifiers> 
parse_base_type(struct context *ctx);

/* 
 * Takes ownership of `base_type`
 *
 * return:
 * 0 -> full type tree (ptr -> arr -> int)
 * 1 -> pointer to the leaf of the tree (arr)
 */
std::tuple<struct type_spec *, struct type_spec *>
parse_type_expr(struct context *ctx,
                struct type_spec *base_type,
                std::optional<struct string_view> *identifier);

struct function parse_func_args(struct context *ctx);

#endif // PCC_PARSE_TYPES_H
