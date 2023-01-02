#include "matrix"
#include <iostream>

int main() {
    Matrix2D<double> dsa{
        {1.1,2.1,3.1,4.1},
        {2.1,3.1,2.1,1.1},
        {1.1,1.1,1.1,1.1}
    };
    dsa.row_operation(2, [](auto a){return a*200;});
    (dsa).display();
}