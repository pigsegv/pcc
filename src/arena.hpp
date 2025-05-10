#ifndef UTIL_ARENA_H
#define UTIL_ARENA_H

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <new>

#define ARENA_DEF_BLOCK_SIZE 4096

#define arena_save(arena, r)
#define arena_restore(arena, r)

namespace pcc {
typedef size_t save_state;

class arena {
public:
  arena(size_t block_size = ARENA_DEF_BLOCK_SIZE);
  ~arena(void);

  template <typename T>
  T *alloc(size_t size);

  char &operator [](size_t index);
  save_state save(void);
  void restore(save_state state);

private:
  size_t m_allocated;
  const size_t m_capacity;

  char *m_block; // Contains both the data and the next node

  class arena *m_next;
  char *m_data;
};


template <typename T>
T *arena::alloc(size_t size) {
  if (m_allocated + size > m_capacity) {
    if (m_next == nullptr) {
      m_next = new (m_block) arena(m_capacity);
    }

    return m_next->alloc<T>(size);
  }

  m_allocated += size;
  return m_data + m_allocated;
}

}


#endif // UTIL_ARENA_H
