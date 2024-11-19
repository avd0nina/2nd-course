#include <stdio.h>
#include <iostream>
#include <chrono>

#define CACHE_SIZE 32 * 1024
#define NUM_OF_RUNS 8

int* GenerateArray(int blocks_num) {
    int cache = CACHE_SIZE;
    int block_size = CACHE_SIZE / blocks_num;
    int* array = new int[cache * blocks_num / sizeof(int)];
    for (int i = 0; i < blocks_num; i++) {
        for (int j = 0; j < block_size / sizeof(int); j++) {
            int current_index = cache / sizeof(int) * i + j;
            if (i == blocks_num - 1) {
                array[current_index] = (j + 1) % (block_size / sizeof(int));
            }
            else {
                array[current_index] = current_index + cache / sizeof(int);
            }
        }
    }
    return array;
}

long AccessTime(int* array) {
    int elements_num = CACHE_SIZE / sizeof(int);
    int volatile index_of_next = 0;
    for (int i = 0; i < elements_num; i++) {
        index_of_next = array[index_of_next];
    }
    index_of_next = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < elements_num * 8; i++) {
        index_of_next = array[index_of_next];
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    long average_time = duration.count() / (elements_num * 8);
    return average_time;
}

int main() {
    for (int blocks_num = 1; blocks_num <= 32; blocks_num++) {
        int* array = GenerateArray(blocks_num);
        long average_time = 0;
        for (int i = 0; i < NUM_OF_RUNS; i++) {
            average_time += AccessTime(array);
        }
        std::cout << "Number of blocks: " << blocks_num << "; average time: " << average_time / NUM_OF_RUNS << " ns\n";
        delete[] array;
    }
}
