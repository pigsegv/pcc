#ifndef UTIL_ARENA_H
#define UTIL_ARENA_H

#include "constraints.hpp"

#include <new>
#include <cstdlib>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <cstdio>
#include <string>

#define ARENA_DEF_BLOCK_SIZE 4096

class arena {
public:
  arena(size_t block_size = ARENA_DEF_BLOCK_SIZE);
  ~arena(void);

  void *alloc(size_t size);

  char &operator [](size_t index);
  size_t save(void);
  void restore(size_t state);
  void reset(void);

  size_t get_block_size(void);

  bool is_locked(void);

private:
  void lock(void);
  void unlock(void);

  class arena *get_node(uint64_t node);

private:
  size_t m_allocated;
  const size_t m_capacity;

  char *m_block; // Contains both the data and the next node

  class arena *m_next;
  char *m_data;

  bool m_locked = false;
};

#endif // UTIL_ARENA_H
