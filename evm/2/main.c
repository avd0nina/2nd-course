#include <stdio.h>
#include <stdlib.h>

void Swap(int *a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void Generate(int *a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 100;
    }
}

void BubbleSort(int* a, int n){
    for (int i = 0; i < (n - 1); i++) {
        for (int j = 0; j < (n - i - 1); j++) {
            if (a[j] > a[j + 1]) {
                Swap(&a[j], &a[j + 1]);
            }
        }
    }
}

int main(void) {
    int N;
    if (scanf("%d", &N)) {
        int* a = malloc(sizeof(*a) * N);
        if (!a) {
            return -1;
        }
        Generate(a, N);
        BubbleSort(a, N);
        free(a);
    }
    return 0;
}

