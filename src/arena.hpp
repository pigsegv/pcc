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
class arena_allocator {
public:
  using value_type      = T;
  using pointer         = T *;
  using const_pointer   = const T *;
  using reference       = T &;
  using const_reference = const T &;
  using size_type       = size_t;
  using difference_type = ptrdiff_t;
  
  arena_allocator() : m_arena() { }

  pointer allocate(size_type n) {
    return m_arena.alloc<value_type>(n * sizeof(value_type));
  }

  void deallocate(pointer p, size_type n) { };
  void destroy(pointer);

  bool operator ==(class arena_allocator<value_type> &) {
    return false;
  };
  bool operator !=(class arena_allocator<value_type> &) {
    return true;
  };

private:
  class arena m_arena;
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

using arena_str = std::basic_string<char, std::char_traits<char>,
                                    arena_allocator<char>>;

#endif // UTIL_ARENA_H
