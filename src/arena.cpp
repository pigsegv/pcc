#include "arena.hpp"

#include <stdint.h>
#include <cstdio>
#include <cstdlib>

size_t arena::get_block_size(void) {
  return m_capacity;
}

uint64_t arena::save(void) {
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
  uint64_t node = state / m_capacity;
  uint64_t offset = state % m_capacity;

  class arena *a = get_node(node);
  
  class arena *tmp = a->m_next;
  while (tmp != nullptr) { // TODO: Don't traverse the entire list
    tmp->m_allocated = 0;
    tmp = tmp->m_next;
  }

  a->m_allocated = offset;
}

void arena::reset(void) {
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

char &arena::operator [](size_t index) {
  uint64_t node = index / m_capacity;
  uint64_t offset = index % m_capacity;
  
  return get_node(node)->m_data[offset];
}

arena::arena(size_t block_size) : m_capacity(block_size){
  m_block = new char[block_size + sizeof(class arena)];

  m_next = nullptr;
  m_data = m_block + sizeof(*m_next);

  m_allocated = 0;
}

arena::~arena(void) {
  if (m_next != nullptr) {
    m_next->~arena();
  }

  delete[] m_block;

  m_block = nullptr;
  m_next = nullptr;
  m_data = nullptr;

  m_allocated = 0;
}
