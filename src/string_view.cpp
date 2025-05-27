#include "string_view.hpp"

#include <stdint.h>

int sv_cmp(const struct string_view *sv1, const struct string_view *sv2) {
  for (uint64_t i = 0; i < sv2->len; i++) {
    if (i >= sv1->len) {
      return 0;
    }

    if (sv1->view[i] != sv2->view[i]) {
      return *(const unsigned char *)&sv1->view[i] -
             *(const unsigned char *)&sv2->view[i];
    }
  }

  return 0;
}
