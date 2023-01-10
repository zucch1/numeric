#pragma once
#include <initializer_list>
#include <memory>

#define Det(a) (a.det())

template <typename U>
using binary_operator_fn = U(*)(U,U);
template <typename U>
using unary_operator_fn = U(*)(U);


template <typename T>
class Matrix2D {
private:
    std::unique_ptr<T[]> rows;
    std::unique_ptr<T*[]> data;
    // T** data;
    unsigned n_elems;   // M x N
    unsigned M;         
    unsigned N;
public:
    Matrix2D(std::initializer_list<std::initializer_list<T>> list);
    Matrix2D(const unsigned M, const unsigned N);
    Matrix2D(const Matrix2D<T>&);
    Matrix2D<T>& operator=(const Matrix2D<T>&);
    Matrix2D(Matrix2D<T>&&) noexcept;    // don't cast to rvalue ref
    Matrix2D<T>& operator=(Matrix2D<T>&&) noexcept;

    void set(unsigned i, unsigned j, T elem);
    T get(unsigned i, unsigned j) const;
    int det() const;
    void fill(T elem);
    Matrix2D<T> submatrix(unsigned i, unsigned j, unsigned m, unsigned n) const;
    Matrix2D<T> transpose();
    Matrix2D<T> inverse() const;
    void row_swap(unsigned i, unsigned j);
    void row_multiply(unsigned i, T);
    void row_divide(unsigned i, T);
    void row_add(unsigned i, T);
    void row_subtract(unsigned i, T);
    void row_operation(unsigned i, unary_operator_fn<T>);
    void column_swap(unsigned i, unsigned j);
    void column_multiply(unsigned i, T);
    void column_divide(unsigned i, T);
    void column_add(unsigned i, T);
    void column_subtract(unsigned i, T);
    void column_operation(unsigned i, unary_operator_fn<T>);
//! Still to implement
    std::initializer_list<T> row(unsigned i);
    void row_operation(unsigned i, unsigned j, binary_operator_fn<T>);
    void column_operation(unsigned i, unsigned j, binary_operator_fn<T>);
    std::initializer_list<T> column(unsigned i);
    void add_rows(unsigned i, unsigned j);
    void subtract_rows(unsigned i, unsigned j);
    void add_columns(unsigned i, unsigned j);
    void subtract_columns(unsigned i, unsigned j);
    template <typename U>
    operator Matrix2D<U>() const;
//! End

    bool operator==(const Matrix2D<T>&) const;       // Matrices are equal if their elements are equal
    bool operator!=(const Matrix2D<T>&) const;

    static_assert(std::is_same<decltype(T{} + T{}), T>::value, "T must support multiplication");
    static_assert(std::is_same<decltype(T{} - T{}), T>::value, "T must support subtraction");
    static_assert(std::is_same<decltype(T{} * T{}), T>::value, "T must support multiplication");
    static_assert(std::is_same<decltype(T{} * 3), T>::value, "T must be scalable");

    Matrix2D<T> operator|(const Matrix2D<T>&) const;
    Matrix2D<T> operator+(const Matrix2D<T>&) const;
    Matrix2D<T> operator-(const Matrix2D<T>&) const;
    Matrix2D<T> operator*(const Matrix2D<T>&) const;
    Matrix2D<T> operator/(const Matrix2D<T>&) const;
    T* operator[](unsigned i) const;
    T** operator()() const;
    operator const T**() const;
    virtual ~Matrix2D() = default;
    static Matrix2D<T> I(unsigned short I);
    void display() const;
    inline unsigned m(){ return M;}
    inline unsigned n(){ return N;}

    template <typename U, typename P>
    friend Matrix2D<U> operator*(Matrix2D<U>& other, P n);
    template <typename U, typename P>
    friend Matrix2D<U> operator*(P n, Matrix2D<U>& other);
};

template <typename T>
void GaussJordan(Matrix2D<T>& A, Matrix2D<T>& b);