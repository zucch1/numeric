#include "matrix.hpp"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <type_traits>

template <typename T>
// TODO: Prevent a T** from being assigned to Matrix2D(unsafe)
// TODO: Do checks for null matrices, moved-from matrices and incompatible matrices
// TODO: Format Error messages

Matrix2D<T>::Matrix2D(const unsigned rows, const unsigned cols) : M{rows}, N{cols}{
    try{
        if(M < 1 || N < 1) throw std::invalid_argument("Invalid Matrix Order");    // Only positive order matrices
        n_elems = M*N;
        data = std::make_unique<T*[]>(M); //(T**)malloc(M * sizeof(T*));     // Array of M rows
        // Allocate rows(T *) and set data 
        this->rows = std::make_unique<T[]>(n_elems); //new T[n_elems];//(T*) malloc((n_elems * sizeof(T)));
        // memset(row, 0, n_elems);
        // initialse the rows to default
        std::fill(this->rows.get(), this->rows.get()+n_elems, T{0}); //! T{} may need to be copied
        for(unsigned i = 0; i < M; i++){   
            data[i] = &this->rows[i*N];
        }
    } catch (...) {
        std::cerr<< "Error creating " << M << "x" << N <<" Matrix!" << '\n';
    }
}

template <typename T>
Matrix2D<T>::Matrix2D(std::initializer_list<std::initializer_list<T>> list) {
    unsigned cols = list.begin()->size();   // length of first list (should be equal in all lists)
    for(unsigned i = 0; i < list.size(); i++){
        if((list.begin()+i)->size() != cols) throw std::invalid_argument("Non-uniform initializer list");
    }
    M = list.size();
    N = list.begin()->size();
    n_elems = M * N;
    data = std::make_unique<T*[]>(M);
    rows = std::make_unique<T[]>(n_elems);
    for (unsigned i = 0; i < M; i++) {
        data[i] = &rows[i * N];
    }
    unsigned i = 0, j = 0;
    for (auto row : list) {
        j = 0;
        for (auto elem : row) {
            set(i, j, elem);
            j++;
        }
        i++;
    }
}

template<typename T>
Matrix2D<T>::Matrix2D(const Matrix2D<T>& other){
    M = other.M;
    N = other.N;
    n_elems = M*N;
    data = std::make_unique<T*[]>(M);
    rows = std::make_unique<T[]>(n_elems);
    for(unsigned int i = 0; i < M; i++){   
        data[i] = &rows[i*N];
    }
    for(unsigned i = 0; i < this->M; i++){
        for(unsigned j = 0; j < this->N; j++){
            set(i, j, other.get(i, j));
        }
    }
}

template<typename T>
Matrix2D<T>& Matrix2D<T>::operator=(const Matrix2D<T>& other){
    if(this->data == other.data) return *this;
    if(this->M != other.M || this->N != other.M) throw std::invalid_argument("Matrix2D order mismatch");
    if(this->data == nullptr || other.data == nullptr) throw std::invalid_argument("Invalid Matrix");
    for(unsigned i = 0; i < this->M; i++){
        for(unsigned j = 0; j < this->N; j++){
            this->set(i, j, other.data[i][j]);
        }
    }
    return *this;
}

// Don't use std::move on Matrix2D
template<typename T>
Matrix2D<T>::Matrix2D(Matrix2D<T>&& other) noexcept {
    data.reset(other.data.release());
    rows.reset(other.rows.release());
    this->M = other.M;
    this->N = other.N;
    this->n_elems = M*N;
    other.M = 0;
    other.N = 0;
    other.n_elems = 0;
}

template<typename T>
Matrix2D<T>& Matrix2D<T>::operator=(Matrix2D<T>&& other) noexcept {
    if(this->data == other.data || this->M != other.M || this->N != other.M) return *this;
    this->data.reset(other.data.release());
    this->rows.reset(other.rows.release());
    this->M = other.M;
    this->N = other.N;
    this->n_elems = M*N;
    other.M = 0;
    other.N = 0;
    other.n_elems = 0;
    return *this;
}


template <typename T>
void Matrix2D<T>::set(unsigned i, unsigned j, T elem){
    if(i >= M || j >= N){
        throw std::out_of_range("Out of bounds!");
    }
    if(data == nullptr) throw std::invalid_argument("Invalid Matrix");
    data[i][j] = elem;
}

template <typename T>
T Matrix2D<T>::get(unsigned i, unsigned j) const{
    if(i >= M || j >= N) throw std::out_of_range("Out Of Bounds");
    if(data == nullptr) throw std::invalid_argument("Invalid Matrix");
    return data[i][j];    
}

template <typename T>
void Matrix2D<T>::fill(T elem){
    if(data.get() == nullptr) return;
    if(rows.get() == nullptr) return;
    std::fill(rows.get(), rows.get() + n_elems, elem);
    // for(unsigned i = 0; i < M; i++){
    //     for(unsigned j = 0; j < N; j++){
    //         set(i, j, elem);
    //     }
    // }
}



// template <typename T>
// int Matrix2D<T>::det(){

// }

template <typename T>
Matrix2D<T> Matrix2D<T>::transpose(){
    Matrix2D<T> result{N,M};
    for(unsigned i = 0; i < N; i++){
        for(unsigned j = 0; j < M; j++){
            result.set(i, j, get(j, i));
        }
    }
    return result;
}

template <typename T>
Matrix2D<T> Matrix2D<T>::submatrix(unsigned i, unsigned j, unsigned m, unsigned n) const{
    if(i+m>M || j+n>N){
        throw std::invalid_argument("Submatrix larger than matrix");
    }
    Matrix2D<T> sub{m,n};
    for(int I = 0; I < m; I++){
        for(int J = 0; J < n; J++){
            sub.set(I, J, get(i+I, j+J));
        }
    }
    return sub;
}

// Returns an augmented matrix
template <typename T>
Matrix2D<T> Matrix2D<T>::operator|(const Matrix2D<T>& other) const {
    if(this->M != other.M)
        throw std::invalid_argument("Row mismatch: The matrices are not compatible for augmentation!");
    if(!this->data)
        throw std::invalid_argument("Null Matrix: Invalid Left Operand for matrix augmentation!");
    if(!other.data)
        throw std::invalid_argument("Null Matrix: Invalid Right Operand for matrix augmentation!");

    Matrix2D<T> result{M, this->N + other.N};
    for(unsigned i = 0; i < this->M; i++){
        for(unsigned j = 0; j < this->N; j++){
            result.set(i, j, this->get(i, j));
        }
        for(unsigned j = 0; j < other.N; j++){
            result.set(i, j+this->N, other.get(i, j));
        }
    }
    return result;
}


template<typename T>
bool Matrix2D<T>::operator==(const Matrix2D<T>& other) const {
    if(this->M != other.M || this->N != other.N) return false;
    for(unsigned int i = 0; i < M; i++){
        for(unsigned int j = 0; j < N; j++){
            if(this->get(i, j) != other.get(i, j)) return false;
        }
    }
    return true;
}

template<typename T>
bool Matrix2D<T>::operator!=(const Matrix2D<T>& other) const {
    return !(*this == other);
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator+(const Matrix2D<T>& other) const {
    static_assert(std::is_same<decltype(T{} + T{}), T>::value, "T must support addition");
    if(this->M != other.M || this->N != other.N) throw std::invalid_argument("Matrix order not equal");
    Matrix2D<T> result{M, N};
    for(unsigned int i = 0; i < this->M; i++){
        for(unsigned int j = 0; j < this->N; j++){
            result.set(i, j, (this->get(i, j) + other.get(i, j)));
        }
    }
    return result;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator-(const Matrix2D<T>& other) const {
    static_assert(std::is_same<decltype(T{} - T{}), T>::value, "T must support subtraction");
    Matrix2D<T> temp{other.M, other.N};
    if(this->M != other.M || this->N != other.N) throw "Matrix order not equal";
    for(unsigned int i = 0; i < this->M; i++){
        for(unsigned int j = 0; j < this->N; j++){
            temp.set(i, j, (this->get(i, j) - other.get(i, j)));
        }
    }
    return temp;
}

// Standard Matrix multiplication
template<typename T>
Matrix2D<T> Matrix2D<T>::operator*(const Matrix2D<T>& other) const {
    static_assert(std::is_same<decltype(T{} + T{}), T>::value, "T must support multiplication");
    if(this->N != other.M) throw std::invalid_argument("Matrices are not product compatible");
    Matrix2D<T> result{this->M, other.N};
    // Standard matrix multiplication
    for(int i = 0; i < this->M; i++){
        for(int j = 0; j < other.N; j++){
            T sum{0};
            for (int k = 0; k < this->N; k++) {
                sum += this->get(i, k) * other.get(k, j);
            }
            result.set(i, j, sum);
        }
    }
    return result;
}


// Scalar post multiplication 
template <typename T, typename P>
Matrix2D<T> operator*(Matrix2D<T>& other, P n) {
    Matrix2D<T> result{other.M, other.N};
    for(unsigned i = 0; i < other.M; i++){
        for(unsigned j = 0; j < other.N; j++){
            result.set(i, j, n * other.get(i, j));
        }
    }
    return result;
}

// Scalar pre multiplication 
template <typename T, typename P>
Matrix2D<T> operator*(P n, Matrix2D<T>& other){
    Matrix2D<T> result{other.M,other.N};
    for(unsigned i = 0; i < other.M; i++){
        for(unsigned j = 0; j < other.N; j++){
            result.set(i, j, n * other.get(i, j));
        }
    }
    return result;
}

//! UNSAFE!!
template <typename T>
T* Matrix2D<T>::operator[](unsigned i) const{
        if(i > M) throw std::out_of_range("Row index out of bounds");
        return data[i];
}

//! UNSAFE!! Use with caution!
template <typename T>
T** Matrix2D<T>::operator()() const{
    return data.get();
}

// Returns an NxN identity matrix of type T
template <typename T>
Matrix2D<T> Matrix2D<T>::I(unsigned short N) {
    Matrix2D<T> temp{N, N};
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            temp.set(i, j, (i==j ? T{1} : T{0}));
        }
    }
    return temp;
}

template <typename T>
void Matrix2D<T>::display() const {
    // Find the maximum width of the elements in the matrix
    int max_width = 0;
    for (unsigned short i = 0; i < M; i++) {
        for (unsigned short j = 0; j < N; j++) {
            // Convert the element to a string and find its length
            std::string str = std::to_string(get(i, j));
            int width = str.length();
            if (width > max_width) {
                max_width = width;
            }
        }
    }

    // Output the matrix
    for (unsigned short i = 0; i < M; i++) {
        for (unsigned short j = 0; j < N; j++) {
            // Output the element, padded with spaces to align the columns
            std::cout << std::setw(max_width) << get(i, j) << " ";
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

template <typename T>
void Matrix2D<T>::row_swap(unsigned i, unsigned j) {
    if (i >= M || j >= M) {
        throw std::out_of_range("row_swap::row index out of range");
    }

    std::unique_ptr<T[]> temp = std::make_unique<T[]>(N);
    std::copy(data[i], data[i] + N, temp.get());

    std::copy(data[j], data[j] + N, data[i]);

    std::copy(temp.get(), temp.get() + N, data[j]);
}

template <typename T>
void Matrix2D<T>::row_operation(unsigned i, unary_operator_fn<T> fn){
    if(i >= M) throw std::out_of_range("row_multiply::row index out of range");
    for(unsigned j = 0; j < N; j++){
        set(i, j, fn(get(i, j)));
    }
}

//! Incomplete behaviour
template <typename T>
void Matrix2D<T>::row_operation(unsigned i, unsigned j, binary_operator_fn<T> fn){
    if(i >= M || j >= N) throw std::out_of_range("row_operation::row index out of range");
    for(unsigned k = 0; j < N; j++){
        set(i, j, fn(get(i, k), get(j, k)));
    }
}


template <typename T>
void Matrix2D<T>::row_multiply(unsigned i, T n){
    if(i >= M) throw std::out_of_range("row_multiply::row index out of range");
    for(unsigned j = 0; j < N; j++){
        set(i, j, get(i, j) * n);
    }
}

template <typename T>
void Matrix2D<T>::row_add(unsigned i, T n){
    if(i >= M) throw std::out_of_range("row_add::row index out of range");
    for(unsigned j = 0; j < N; j++){
        set(i, j, n + get(i, j));
    }
}

template <typename T>
void Matrix2D<T>::row_subtract(unsigned i, T n){
    if(i >= M) throw std::out_of_range("row_subtract::row index out of range");
    for(unsigned j = 0; j < N; j++){
        set(i, j, get(i, j) - n);
    }
}


template <typename T>
void Matrix2D<T>::row_divide(unsigned i, T n){
    if(i >= M) throw std::out_of_range("row_divide::row index out of range");
    if(n == T{0}) throw std::invalid_argument("row_divide::division by zero");
    for(unsigned j = 0; j < N; j++){
        set(i, j, get(i, j) / n);
    }
}




template <typename T>
void Matrix2D<T>::column_swap(unsigned i, unsigned j) {
    if (i >= N || j >= N) {
        throw std::out_of_range("column_swap::column index out of range");
    }

    std::unique_ptr<T[]> temp = std::make_unique<T[]>(M);
    for (unsigned short k = 0; k < M; k++) {
        temp[k] = data[k][i];
    }

    for (unsigned short k = 0; k < M; k++) {
        data[k][i] = data[k][j];
    }

    for (unsigned short k = 0; k < M; k++) {
        data[k][j] = temp[k];
    }
}


template <typename T>
void Matrix2D<T>::column_operation(unsigned j, unary_operator_fn<T> fn){
    if(j >= N) throw std::out_of_range("column_operation::column index out of range");
    for(unsigned i = 0; i < M; i++){
        set(i, j, fn(get(i, j)));
    }
}


template <typename T>
void Matrix2D<T>::column_multiply(unsigned j, T n){
    if(j >= N) throw std::out_of_range("column_multiply::column index out of range");
    for(unsigned i = 0; i < M; i++){
        set(i, j, get(i, j) * n);
    }
}

template <typename T>
void Matrix2D<T>::column_add(unsigned j, T n){
    if(j >= N) throw std::out_of_range("column_add::column index out of range");
    for(unsigned i = 0; i < M; i++){
        set(i, j, n + get(i, j));
    }
}

template <typename T>
void Matrix2D<T>::column_subtract(unsigned j, T n){
    if(j >= N) throw std::out_of_range("column_subtract::column index out of range");
    for(unsigned i = 0; i < M; i++){
        set(i, j, get(i, j) - n);
    }
}


template <typename T>
void Matrix2D<T>::column_divide(unsigned j, T n){
    if(j >= N) throw std::out_of_range("column_divide::column index out of range");
    if(n == T{0}) throw std::invalid_argument("column_divide::division by zero");
    for(unsigned i = 0; i < M; i++){
        set(i, j, get(i, j) / n);
    }
}
