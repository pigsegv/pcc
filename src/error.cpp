#include "error.hpp"

#include <cstdio>
#include <stdint.h>
#include <stddef.h>
#include <cstdlib>
#include <cstdio>

namespace pcc {

#define FORMAT_ERROR(x) "\033[31;1m" x "\033[0m"

void report_error(const char *path, const char *start, 
                  const char *loc, const char *msg) {
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

  std::fprintf(stderr, "%s:%u:%lu: " FORMAT_ERROR("error: ") "%s\n", 
               path, line_count, (ptrdiff_t)(loc - line_start) + 1, msg);
  
}

}
