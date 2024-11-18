#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <mach/mach_time.h>

#define _1KB 1024
#define _1MB 1048576

uint64_t get_nanoseconds(uint64_t mach_time) {
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    return mach_time * timebase.numer / timebase.denom;
}

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
    std::vector<int> indices(n);
    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    for (int i = 0; i < n - 1; i++) {
        mas[indices[i]] = indices[i + 1];
    }
    mas[indices[n - 1]] = indices[0];
}

double AverageTime(int* mas, int n, int phases) {
    int volatile next_index = 0;
    uint64_t start = mach_absolute_time();
    for (int i = 0; i < (n * phases); i++) {
        next_index = mas[next_index];
    }
    uint64_t end = mach_absolute_time();
    uint64_t time_ns = get_nanoseconds(end - start);
    return static_cast<double>(time_ns) / (n * phases);
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
    std::printf("Straight - size: %d Kb; time: %.2f ns\n", arr_size / 1024, average_time);
}

void PrintReverseTime(int* mas, int arr_size, int n) {
    Reverse(mas, n);
    AverageTime(mas, n, 1);
    double average_time = AverageTime(mas, n, 4);
    std::printf("Reverse - size: %d Kb; time: %.2f ns\n", arr_size / 1024, average_time);
}

void PrintRandomTime(int* mas, int arr_size, int n) {
    Random(mas, n);
    AverageTime(mas, n, 1); 
    double average_time = AverageTime(mas, n, 4);
    std::printf("Random - size: %d Kb; time: %.2f ns\n", arr_size / 1024, average_time);
}

int main() {
    for (int arr_size = _1KB; arr_size <= _1MB * 32; ReloadSize(arr_size)) {
        size_t n = arr_size / sizeof(int);
        int* mas = new (std::nothrow) int[n];
        if (!mas) {
            std::cerr << "Memory allocation failed for size: " << arr_size << " bytes\n";
            continue;
        }
        PrintStraightTime(mas, arr_size, n);
        PrintReverseTime(mas, arr_size, n);
        PrintRandomTime(mas, arr_size, n);
        std::printf("_\n");
        delete[] mas;
    }
    return 0;
}

