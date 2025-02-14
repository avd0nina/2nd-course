#include <iostream>
#include <random>
#include <ctime>
#include <set>

#define _1KB 1024
#define _1MB 1048576

void Straight(int* mas, int n) {
    for (int i = 0; i < n; i++) {
        mas[i] = i + 1;
    }
    mas[n - 1] = 0;
}

void Reverse(int* mas, int n) {
    for (int i = n - 1; i > 0; i--) {
        mas[i] = i - 1;
    }
    mas[0] = n - 1;
}

void Random(int* mas, int n) {
    std::srand(std::time(nullptr));
    std::set<int> visited_cells;
    int current_index = 0;
    for (int i = 0; i < n - 1; i++) {
        visited_cells.insert(current_index);
        int rand_index = std::rand() % n;
        while (visited_cells.contains(rand_index)) {
            rand_index = std::rand() % n;
        }
        mas[current_index] = rand_index;
        current_index = mas[current_index];
    }
    mas[current_index] = 0;
}

uint64_t rdtsc(){
    unsigned int low, high;
    asm volatile ("rdtsc\n" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

double AverageTime(int* mas, int n, int phases) {
    int volatile next_index = 0;
    uint64_t start = rdtsc();
    for (int i = 0; i < (n * phases); i++) {
        next_index = mas[next_index];
    }
    uint64_t end = rdtsc();
    uint64_t time = end - start;
    return time / (n * phases);
}

void ReloadSize(int& size) {
    if (size == _1KB) {
        size = _1KB * 4;
    } else if (size < _1KB * 32) {
        size += _1KB * 4;
    } else if (size < _1KB * 512) {
        size += _1KB * 16;
    } else if (size < _1MB * 8) {
        size += _1MB * 1;
    } else {
        size += _1MB * 4;
    }
}

void PrintStraightTime(int* mas, int arr_size, int n) {
    Straight(mas, n);
    AverageTime(mas, n, 1);
    double average_time = AverageTime(mas, n, 4);
    std::printf("Straight - size: %d Kb; time: %.2f ticks\n", arr_size / 1024, average_time);
}

void PrintReverseTime(int* mas, int arr_size, int n) {
    Reverse(mas, n);
    AverageTime(mas, n, 1);
    double average_time = AverageTime(mas, n, 4);
    std::printf("Reverse - size: %d Kb; time: %.2f ticks\n", arr_size / 1024, average_time);
}

void PrintRandomTime(int* mas, int arr_size, int n) {
    Random(mas, n);
    AverageTime(mas, n, 1);
    double average_time = AverageTime(mas, n, 4);
    std::printf("Random - size: %d Kb; time: %.2f ticks\n", arr_size / 1024, average_time);
}

int main() {
    for (int arr_size = _1KB; arr_size <= _1MB * 32; ReloadSize(arr_size)) {
        size_t n = arr_size / sizeof(int);
        int* mas = new int[n];
        PrintStraightTime(mas, arr_size, n);
        PrintReverseTime(mas, arr_size, n);
        PrintRandomTime(mas, arr_size, n);
        std::printf("_\n");
        delete[] mas;
    }
    return 0;
}
