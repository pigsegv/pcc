#pragma once

#ifndef PCC_DA_H
#define PCC_DA_H

#include <cstring>
#include <cassert>
#include <iostream>

#define DA_INIT_CAPACITY 256
#define DA_GROW_FACTOR 1.5

template <typename T>
struct dynamic_array {
  dynamic_array() = delete;
  dynamic_array(class arena *arena, 
                uint64_t init = DA_INIT_CAPACITY) : m_arena(arena) {
    m_arena_save = m_arena->save();

    count = 0;
    m_capacity = init;
    m_items = m_arena->alloc<T>(m_capacity * sizeof(*m_items));

    assert(m_items != nullptr && "Failed to allocate memory");
  }

  ~dynamic_array(void) {
    m_arena->restore(m_arena_save);
  }

  void append(T em) {
    if (count >= m_capacity) {
      assert(m_capacity > 0);
      m_capacity = m_capacity * DA_GROW_FACTOR;

      auto tmp = m_arena->alloc<T>(m_capacity * sizeof(*m_items));
      assert(tmp != nullptr && "Failed to allocate memory");

      std::memcpy(tmp, m_items, count * sizeof(*m_items));

      m_items = tmp;
    }

    m_items[count++] = em;
  }

  T &operator [](uint64_t i) {
    return m_items[i];
  }

  uint64_t count;

private:
  T *m_items = nullptr;
  uint64_t m_capacity;

  class arena *m_arena;
  uint64_t m_arena_save;
};

#endif // PCC_DA_H
