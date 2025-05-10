#pragma once
#ifndef UTIL_FILE_IO_H
#define UTIL_FILE_IO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#define FIO_MAX_ITERS 100

// Compatible with Windows
// Sets errno on error
char *fio_read_file(const char *path, uint64_t *size);

// Sets errno on error
int fio_write_to_file(FILE *file, void *data, int size);

#ifdef UTIL_FILE_IO_IMPLEMENTATION

char *fio_read_file(const char *path, uint64_t *size) {
  FILE *f = fopen(path, "rb");

  if (!f) {
    return NULL;
  }

  struct stat s;
  fstat(fileno(f), &s);

  if (size != NULL) {
    *size = s.st_size;
  }

  char *buf = new char[s.st_size + 1];

  int64_t tmp = *size; 
  int i = 0;
  do {
    tmp -= fread(buf, 1, s.st_size, f);

  } while (tmp > 0 && i++ < FIO_MAX_ITERS);

  assert(tmp >= 0);

  *size -= tmp;

  buf[*size] = 0;

  fclose(f);

  return buf;
}

int fio_write_to_file(FILE *file, void *data, int size) {
  errno = 0;
  uint32_t iter = 0;
  do {
    int n = fwrite(data, 1, size, file);

    size -= n;
    data = (char *)data + n;

    iter++;

  } while (iter < FIO_MAX_ITERS && size);

  if (size != 0) {
    return 1;
  }

  return 0;
}

#endif // UTIL_FILE_IO_IMPLEMENTATION

#endif // UTIL_FILE_IO_H
