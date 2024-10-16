#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float Pi(int n) {
    float pi = 0;
    for (int i = 0; i < n; i++) {
        pi += (pow(-1, i) / (2 * i + 1));
    }
    return 4 * pi;
}

int main(void) {
    int N;
    if (scanf("%d", &N)) {
        float result = Pi(N);
        printf("%d %f", N, result);
    }
}
