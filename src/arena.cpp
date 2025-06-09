#include "arena.hpp"

#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <assert.h>
#include <cstddef>
#include <sanitizer/asan_interface.h>

size_t arena::get_block_size(void) {
  return m_capacity;
}

uint64_t arena::save(void) {
  if (m_locked) {
    std::fprintf(stderr, "Attempted to save a locked arena\n");
    std::abort();
  }

  class arena *a = this;

  uint64_t node = 0;
  while (a->m_allocated >= a->m_capacity) {
    if (a->m_next == nullptr) break;

    a = a->m_next;
    node++;
  }

 return node * m_capacity + a->m_allocated; 
}

void arena::restore(uint64_t state) {
  if (m_locked) {
    std::fprintf(stderr, "Attempted to restore a locked arena\n");
    std::abort();
  }

  uint64_t node = state / m_capacity;
  uint64_t offset = state % m_capacity;

  class arena *a = get_node(node);
  
  class arena *tmp = a->m_next;
  while (tmp != nullptr) { // TODO: Don't traverse the entire list
    tmp->m_allocated = 0;
    __asan_poison_memory_region(m_data, m_capacity);
    tmp = tmp->m_next;
  }

  a->m_allocated = offset;
}

void arena::reset(void) {
  if (m_locked) {
    std::fprintf(stderr, "Attempted to reset a locked arena\n");
    std::abort();
  }

  restore(0);
}

class arena *arena::get_node(uint64_t node) {
  class arena *a = this;
  for (uint64_t i = 0; i < node; i++) {
    if (a->m_next == nullptr) {
      std::fprintf(stderr, "Array out of bounds\n");
      std::abort();
    }

    a = a->m_next;
  }

  return a;
}

void arena::lock(void) {
  if (m_locked) {
    std::fprintf(stderr, "Attempted to lock a locked arena\n");
    std::abort();
  }

  m_locked = true;
}

void arena::unlock(void) {
  m_locked = false;
}

bool arena::is_locked(void) {
  return m_locked;
}

char &arena::operator [](size_t index) {
  uint64_t node = index / m_capacity;
  uint64_t offset = index % m_capacity;
  
  return get_node(node)->m_data[offset];
}

void *arena::alloc(size_t size) {
  if (m_locked) {
    std::fprintf(stderr, "Attempted to allocate to a locked arena\n");
    std::abort();
  }

  size_t padding = (uintptr_t)(m_data + m_allocated) % alignof(max_align_t);
  padding = padding == 0 ? 0 : alignof(max_align_t) - padding;

  if (size + padding > m_capacity) return nullptr;

  if (m_allocated + size + padding > m_capacity) {
    if (m_next == nullptr) {
      m_next = new (m_block) arena(m_capacity);
    }

    m_allocated = m_capacity; // Mark this node as 'full'

    return m_next->alloc(size);

  } else {
    void *block = m_data + m_allocated + padding;
    __asan_unpoison_memory_region(block, size);
    m_allocated += size + padding;

    return block;
  }
}

arena::arena(size_t block_size) : m_capacity(block_size) {
  m_block = new char[block_size + sizeof(class arena)];

  m_next = nullptr;
  m_data = m_block + sizeof(*m_next);
  __asan_poison_memory_region(m_data, block_size);

  m_allocated = 0;
}

arena::~arena(void) {
  if (m_next != nullptr) {
    m_next->~arena();
  }

  delete[] m_block;

  m_block = nullptr;
  m_next  = nullptr;
  m_data  = nullptr;

  m_allocated = 0;
}
