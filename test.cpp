
#include <gtest/gtest.h>
#include "matrix"
#include <stdexcept>

TEST(Matrix2D, UnitIdentityMatrix){
    Matrix2D<int> I1 = Matrix2D<int>::I(1);
    EXPECT_EQ(I1(0,0), 1);
}

TEST(Matrix2D, IdentityMatrix){
    Matrix2D<int> A = Matrix2D<int>::I(100);
    for (unsigned i = 0; i < A.m(); i++) {
        for (unsigned j = 0; j < A.n(); j++) {
            if (i == j) {
                EXPECT_EQ(A(i,j), 1);
            } else {
                EXPECT_EQ(A(i,j), 0);
            }
        }
    }
}
