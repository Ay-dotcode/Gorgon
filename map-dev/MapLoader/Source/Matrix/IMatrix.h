#pragma once
#include "BaseMatrix.h"
#include "FMatrix.h"

//Class is going to be written with CRTP Pattern
class IMatrix : public Matrix_Base<IMatrix, int> {
protected:
    // Friending Matrix_Base so we can actually access the implementations without making them public.
    friend Matrix_Base<IMatrix,int>;

    // Matrix it self;
    matrix_t<int> matrix;

    // Implementions of the functions
    IMatrix multiplyImpl(IMatrix &target) const;
    void PrintImpl() const;
    std::vector<int> GetRowImpl(unsigned int);
public:
    // Constructors
    IMatrix(const std::vector<std::vector<int>>& list) : matrix(list) {} // NOLINT

    explicit IMatrix(const matrix_t<float>& list) {
        for(auto x : list) {
            std::vector<int> temp;
            for(auto& y : x) {
                temp.push_back(static_cast<int>(y));
            }
            matrix.push_back(temp);
        }
    }

    explicit operator FMatrix() const {
        return FMatrix(matrix);
    }
    IMatrix() = default;

    // Size operations
    [[nodiscard]] size_t RowSize() const;
    [[nodiscard]] size_t ColumnSize(unsigned int = 0) const;

    // Operator overloads
    [[nodiscard]] matrix_column_t<int> & operator[](unsigned int);
    [[nodiscard]] matrix_column_t<int> operator[](unsigned int) const;
    IMatrix& operator=(matrix_t<int>);
    IMatrix& operator=(matrix_t<float>);
    bool operator==(const IMatrix&) const;

    // Iterators


    matrix_t<int>::iterator begin();
    matrix_t<int>::iterator end();
};

