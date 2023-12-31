//
// Created by theany on 29.12.2023.
//
#pragma once
#include "Matrix/Matrix.h"

namespace IntMatrix_Test {

enum TEST_RESULT : int { PASS, FAIL, PARTIAL_FAIL };

inline std::map<std::array<IMatrix, 2>, IMatrix> test_case;
inline void addTestCase(const std::array<IMatrix, 2> &key, IMatrix result) {
  test_case[key] = std::move(result);
}

inline void removeTestCase(const std::array<IMatrix, 2> &key) {
  test_case.erase(key);
}

inline TEST_RESULT test() {
  for (auto Case : test_case) {
    auto arr = Case.first;
    if ((arr[0] * arr[1]) == Case.second) {
    }
  }
  return PASS;
}
} // namespace IntMatrix_Test
