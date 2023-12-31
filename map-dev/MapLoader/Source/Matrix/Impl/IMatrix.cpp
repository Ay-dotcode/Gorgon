#include "../IMatrix.h"

matrix_column_t<int> &IMatrix::operator[](unsigned int index) {
  return matrix[index];
}
matrix_column_t<int> IMatrix::operator[](unsigned int index) const {
  return matrix[index];
}

size_t IMatrix::RowSize() const { return matrix.size(); }

size_t IMatrix::ColumnSize(unsigned int rowindex) const {
  return matrix[rowindex].size();
}

IMatrix &IMatrix::operator=(matrix_t<int> matrix) {
  this->matrix = std::move(matrix);
  return *this;
}

IMatrix IMatrix::multiplyImpl(IMatrix &target) const {
  IMatrix holder = matrix;
  matrix_t<int> result;
  result.resize(holder.RowSize());
  for (auto &x : result) {
    x.resize(holder.ColumnSize());
  }

  for (int row = 0; row < holder.RowSize(); row++) {
    for (int col = 0; col < holder.ColumnSize(); col++) {
      int res = 0;
      for (int trow = 0; trow < target.RowSize(); trow++) {
        res += holder[row][trow] * target[trow][col];
      }
      result[row][col] = res;
    }
  }
  holder = result;
  return holder;
}

void IMatrix::PrintImpl() const {
  for (auto const &row : this->matrix) {
    std::cout << "{ ";
    for (auto const col : row) {
      std::cout << col << " ";
    }
    std::cout << "}" << std::endl;
  }
}

matrix_t<int>::iterator IMatrix::begin() { return matrix.begin(); }

matrix_t<int>::iterator IMatrix::end() { return matrix.end(); }

bool IMatrix::operator==(const IMatrix &next) const {
  return this->matrix == next.matrix;
}
