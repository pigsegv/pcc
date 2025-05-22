#ifndef UTIL_ARENA_H
#define UTIL_ARENA_H

#include "constraints.hpp"

#include <new>
#include <cstdlib>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <cstdio>

#define ARENA_DEF_BLOCK_SIZE 4096

class arena {
  template <trivially_copyable T>
  friend class dynamic_array;
public:
  arena(size_t block_size = ARENA_DEF_BLOCK_SIZE);
  ~arena(void);

  template <typename T>
  T *alloc(size_t size);

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

template <typename T>
T *arena::alloc(size_t size) {
  if (m_locked) {
    std::fprintf(stderr, "Attempted to allocate to a locked arena\n");
    std::abort();
  }

  if (size > m_capacity) return nullptr;

  if (m_allocated + size > m_capacity) {
    if (m_next == nullptr) {
      m_next = new (m_block) arena(m_capacity);
    }

    m_allocated = m_capacity; // Mark this node as 'full'

    return m_next->alloc<T>(size);

  } else {
    T *block = (T *)(m_data + m_allocated);
    m_allocated += size;
    return block;
  }
}

#endif // UTIL_ARENA_H
