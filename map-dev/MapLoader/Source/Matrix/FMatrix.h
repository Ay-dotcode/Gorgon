#pragma once
#include "BaseMatrix.h"

class FMatrix : public Matrix_Base<FMatrix, float> {
protected:
    friend Matrix_Base<FMatrix, float>;
    matrix_t<float> matrix;

    FMatrix multiplyImpl(FMatrix &target) const;
    void PrintImpl() const;
    // matrix_column_t<float> GetRowImpl(unsigned int);

public:
    FMatrix(const matrix_t<float>& list) : matrix(list) {} // NOLINT

    explicit FMatrix(const matrix_t<int>& list) {
        for(auto x : list) {
            std::vector<float> temp;
            for(auto& y : x) {
                temp.push_back(static_cast<float>(y));
            }
            matrix.push_back(temp);
        }
    }

    FMatrix() = default;

    [[nodiscard]] size_t RowSize() const;
    [[nodiscard]] size_t ColumnSize(unsigned int = 0) const;

    // Operator Overloads
    [[nodiscard]] matrix_column_t<float> & operator[](unsigned int);
    [[nodiscard]] matrix_column_t<float> operator[](unsigned int) const;
    FMatrix& operator=(matrix_t<float>);
    FMatrix& operator=(matrix_t<int>);
    bool operator==(const FMatrix&);

};
