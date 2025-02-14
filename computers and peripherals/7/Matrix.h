#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
private:
    int m_size;
    float* mas;
public:
    Matrix(int size);
    Matrix(const Matrix& matrix2);
    ~Matrix();
    Matrix Transpose() const;
    Matrix& operator=(const Matrix& matrix2);
    Matrix& operator+=(const Matrix& matrix2);
    Matrix& operator-=(const Matrix& matrix2);
    Matrix& operator/=(float n);
    Matrix operator*(const Matrix& matrix2) const;
    int size() const;
    float* data() const;
    void FillIdentity();
    void FillRandom();
    void PrintMatrix() const;
    Matrix Reverse(int cnt) const;
    friend Matrix GetB(const Matrix& A);
    friend Matrix GetR(const Matrix& A, const Matrix& B);
    friend float GetA1(const Matrix& A);
    friend float GetAinf(const Matrix& A);
};

#endif // MATRIX_H

