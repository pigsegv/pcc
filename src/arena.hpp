#ifndef UTIL_ARENA_H
#define UTIL_ARENA_H

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <new>

#define ARENA_DEF_BLOCK_SIZE 4096

namespace pcc {

class arena {
public:
  arena(size_t block_size = ARENA_DEF_BLOCK_SIZE);
  ~arena(void);

  template <typename T>
  T *alloc(size_t size);

  char &operator [](size_t index);
  size_t save(void);
  void restore(size_t state);

  size_t get_block_size(void);

private:
  class arena *get_node(uint64_t node);

private:
  size_t m_allocated;
  const size_t m_capacity;

  char *m_block; // Contains both the data and the next node

  class arena *m_next;
  char *m_data;
};

template <class T>
class arena_allocator {
public:
  arena_allocator(void) noexcept;

private:
  class arena arena;
};


template <typename T>
T *arena::alloc(size_t size) {
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

}


#endif // UTIL_ARENA_H
