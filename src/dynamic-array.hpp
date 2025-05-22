#pragma once

#ifndef PCC_DA_H
#define PCC_DA_H

#include "arena.hpp"
#include "constraints.hpp"

#include <cstdio>
#include <cstring>
#include <cassert>
#include <iostream>
#include <cstdlib>

#define DA_INIT_CAPACITY 256
#define DA_GROW_FACTOR 1.5

template <trivially_copyable T>
struct dynamic_array {
  dynamic_array() = delete;
  dynamic_array(class arena *arena, uint64_t init = DA_INIT_CAPACITY) 
    : m_uses_arena(true), m_arena(arena) {
    m_arena_save = m_arena->save();

    count = 0;
    m_capacity = init;
    if (m_capacity > arena->get_block_size()) {
      m_uses_arena = false;
      m_items = (T *)std::malloc(m_capacity * sizeof(*m_items));
      m_arena = nullptr;

    } else {
      m_items = m_arena->alloc<T>(m_capacity * sizeof(*m_items));
      m_arena->lock();
    }

    assert(m_items != nullptr && "Failed to allocate memory");
  }

  ~dynamic_array(void) {
    if (m_uses_arena) {
      m_arena->unlock();
      m_arena->restore(m_arena_save);
      m_items = nullptr;

    } else {
      std::free(m_items);
      m_items = nullptr;
    }
  }

  void append(T em) {
    if (count >= m_capacity) {
      assert(m_capacity > 0);
      m_capacity = m_capacity * DA_GROW_FACTOR;

      if (m_uses_arena) {
        if (m_capacity > m_arena->get_block_size()) {
          m_uses_arena = false;
          m_arena->unlock();
          m_arena->restore(m_arena_save);
          m_arena = nullptr;
          m_items = (T *)std::realloc(m_items, m_capacity * sizeof(*m_items));

        } else {
          m_arena->unlock();
          auto tmp = m_arena->alloc<T>(m_capacity * sizeof(*m_items));
          m_arena->lock();

          std::memcpy(tmp, m_items, count * sizeof(*m_items));
          m_items = tmp;
        }

      } else {
        m_items = (T *)std::realloc(m_items, m_capacity * sizeof(*m_items));
      }
    }

    assert(m_items != nullptr && "Failed to allocate memory");
    m_items[count++] = em;
  }

  T &operator [](uint64_t i) {
    assert(m_items != nullptr);
    assert(i < count);

    return m_items[i];
  }

  uint64_t count;

private:
  bool m_uses_arena;
  T *m_items = nullptr;
  uint64_t m_capacity;

  class arena *m_arena;
  uint64_t m_arena_save;
};

#endif // PCC_DA_H
