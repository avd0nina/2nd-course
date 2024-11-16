#include <iostream>
#include <string>
#include "Matrix.h"

int main(int argc, char** argv) {
    int N = std::stoi(std::string(argv[1]));
    int M = std::stoi(std::string(argv[2]));
    Matrix matrix(N);
    matrix.FillRandom();
    Matrix matrix_rev = matrix.Reverse(M);
    return 0;
}
