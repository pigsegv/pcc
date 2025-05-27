#pragma once

#ifndef PCC_SV_H
#define PCC_SV_H

#include <stdint.h>
#include <cstring>

#define TO_SV(str)           \
  {                          \
    .view = str,             \
    .len = std::strlen(str), \
  }

struct string_view {
  const char *view;
  uint64_t len;
};

int sv_cmp(const struct string_view *sv1, const struct string_view *sv2);

#endif // PCC_SV_H
