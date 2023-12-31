#pragma once
#include "MatrixLiterals.h" // NOLINT
#include <iostream>
#include <string>

template <class Derived, typename T> class Matrix_Base {
public:
  Derived multiply(Matrix_Base &target) {
    return static_cast<Derived *>(this)->multiplyImpl(
        static_cast<Derived &>(target));
  }

  void Print(std::string name = std::string("")) {
    if (!name.empty()) {
      std::cout << name << std::endl;
    }
    static_cast<Derived *>(this)->PrintImpl();
  }

  Matrix_Base() = default;
};
