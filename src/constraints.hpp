#pragma once

#ifndef PCC_CONSTRAINTS_H
#define PCC_CONSTRAINTS_H

#include <type_traits>

template <typename T>
concept trivially_copyable = requires (T a) {
  requires std::is_trivially_copyable<T>::value;
};

#endif // PCC_CONSTRAINTS_H
