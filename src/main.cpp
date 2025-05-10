#define UTIL_FILE_IO_IMPLEMENTATION
#include "file-io.hpp"

#include <cstdio>
#include <iostream>

int main(int argc, char **argv) {
  // TODO: Use a more robust arguments parser
  if (argc <= 2) {
    std::fprintf(stderr, "Usage: %s INFILE OUTFILE\n", argv[0]);
    return 1;
  }

  uint64_t size;
  char *contents = fio_read_file(argv[1], &size);
  if (contents == NULL) {
    perror("Failed to read file");
    return 1;
  }

  std::cout << contents;

  delete[] contents;
  return 0;
}
