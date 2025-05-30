#pragma once

#ifndef PCC_SV_H
#define PCC_SV_H

#include <stdint.h>
#include <cstring>
#include <string_view>
#include <string>
#include <unordered_map>
#include <functional>

#define TO_STD_SV(str) std::string_view((str).view, (str).len)

struct string_view {
  const char *view;
  uint64_t len;
};

struct sv_map_hash {
  using is_transparent = void;

  size_t operator ()(const char *key) const noexcept {
    std::string_view &&view = std::string_view(key, std::strlen(key));
    return std::hash<std::string_view>{}(view);
  }

  size_t operator ()(const std::string_view &key) const noexcept {
    return std::hash<std::string_view>{}(key);
  }

  size_t operator ()(const std::string &key) const noexcept {
    return std::hash<std::string>{}(key);
  }
};

template <typename T>
using cstr_umap = std::unordered_map<const char *, T, sv_map_hash,
                                     std::equal_to<>>;

using sv_map = std::unordered_map<const char *, struct string_view,
                                  sv_map_hash, std::equal_to<>>;

int sv_cmp(const struct string_view *sv1, const struct string_view *sv2);

#endif // PCC_SV_H
