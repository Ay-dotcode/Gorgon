#include "../FMatrix.h"
#include <algorithm>
#include <utility>

matrix_column_t<float> &FMatrix::operator[](unsigned int index) {
  return matrix[index];
}

matrix_column_t<float> FMatrix::operator[](unsigned int index) const {
  return matrix[index];
}

FMatrix &FMatrix::operator=(matrix_t<float> matrix) {
  this->matrix = std::move(matrix);
  return *this;
}

size_t FMatrix::RowSize() const { return matrix.size(); }

size_t FMatrix::ColumnSize(unsigned int RowIndex) const {
  return matrix[RowIndex].size();
}

FMatrix FMatrix::multiplyImpl(FMatrix &target) const {
  FMatrix holder = matrix;
  matrix_t<float> result;

  result.resize(holder.RowSize());
  for (auto &x : result) {
    x.resize(holder.ColumnSize());
  }

  for (int row = 0; row < holder.RowSize(); row++) {
    for (int col = 0; col < holder.ColumnSize(); col++) {
      float res = 0;
      for (int trow = 0; trow < target.RowSize(); trow++) {
        res += holder[row][trow] * target[trow][col];
      }
      result[row][col] = res;
    }
  }
  holder = result;
  return holder;
}

void FMatrix::PrintImpl() const {
  for (auto row : this->matrix) {
    std::cout << "{ ";
    for (auto col : row) {
      std::cout << col << " ";
    }
    std::cout << "}" << std::endl;
  }
}

FMatrix &FMatrix::operator=(matrix_t<int> matrix) {
  std::for_each(matrix.begin(), matrix.end(),
                [this](const std::vector<int> &data) -> void {
                  matrix_column_t<float> holder;
                  for (auto x : data) {
                    holder.push_back(static_cast<float>(x));
                  }
                  this->matrix.push_back(holder);
                });
  return *this;
}
