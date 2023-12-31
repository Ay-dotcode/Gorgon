#pragma once

#include <Gorgon/Input/Keyboard.h>

#include "BaseMatrix.h"
#include "FMatrix.h"
#include "IMatrix.h"

inline FMatrix operator*(FMatrix& left, FMatrix right) {
    return left.multiply(right);
}

inline FMatrix operator*(FMatrix& left, IMatrix& right) {
    FMatrix m(right);
    return left.multiply(m);
}

inline IMatrix operator*(IMatrix& left, IMatrix right) {
    return left.multiply(right);
}

inline IMatrix operator*(IMatrix& left, FMatrix right) {
    matrix_t<int> holder;
    for(int row = 0; row < right.RowSize(); row++) {
        holder.emplace_back();
        for(int col = 0; col < right.ColumnSize(); col++) {
            holder[row].push_back(static_cast<int>(right[row][col]));
        }
    }
    IMatrix m = holder;
    return left.multiply(m);
}