#include "arena.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace pcc {


char &arena::operator [](size_t index) {
  uint64_t node = index / m_capacity;
  uint64_t offset = index % m_capacity;
  
  class arena *a = this;
  for (uint64_t i = 0; i < node; i++) {
    if (a->m_next == nullptr) {
      std::fprintf(stderr, "Array out of bounds\n");
      std::abort();
    }

    a = a->m_next;
  }

  return a->m_data[offset];
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

  m_block = nullptr;
  m_next = nullptr;
  m_data = nullptr;

  m_allocated = 0;

  delete m_block;
}

}
