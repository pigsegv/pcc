#include "error.hpp"

#include <cstdio>
#include <stdint.h>
#include <stddef.h>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

void report_error(const char *path, const char *start, 
                  const char *loc, const char *fmt, ...) {
  const char *tmp = loc - 1;
  const char *line_start = nullptr;
  uint32_t line_count = 1;
  
  while (tmp > start) {
    if (*tmp == '\n') {
      if (line_start == nullptr) line_start = tmp + 1;
      line_count++;
    }

    tmp--;
  }
  if (line_start == nullptr) {
    if (tmp < start) line_start = start;
    else line_start = tmp;
  }

  std::fprintf(stderr, "%s:%u:%lu: " FORMAT_ERROR("error: "), 
               path, line_count, (ptrdiff_t)(loc - line_start) + 1);

  std::va_list args;
  va_start(args, fmt);
  std::vfprintf(stderr, fmt, args);
}
