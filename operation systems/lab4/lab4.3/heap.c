#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define HEAP_SIZE (1024 * 1024)
#define ALIGN8(x) (((x) + 7) & ~7) // для выравнивания размера памяти по 8 байтам

typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
} Block;

void *heap_start = NULL;
Block *free_list = NULL;

void InitializeHeap() {
    heap_start = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (heap_start == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    free_list = (Block *)heap_start;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
}

Block *FindFreeBlock(size_t size) {
    Block *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void SplitBlock(Block *block, size_t size) {
    if (block->size >= size + sizeof(Block) + 8) {
        Block *new_block = (Block *)((char *)block + sizeof(Block) + size);
        new_block->size = block->size - size - sizeof(Block);
        new_block->free = 1;
        new_block->next = block->next;
        block->size = size;
        block->next = new_block;
    }
}

void *MyMalloc(size_t size) {
    if (!heap_start) {
        InitializeHeap();
    }
    size = ALIGN8(size);
    Block *block = FindFreeBlock(size);
    if (!block) {
        return NULL;
    }
    block->free = 0;
    SplitBlock(block, size);
    return (char *)block + sizeof(Block);
}

void MyFree(void *ptr) {
    if (!ptr) {
        return;
    }
    Block *block = (Block *)((char *)ptr - sizeof(Block));
    block->free = 1;
    Block *curr = free_list;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(Block) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

int main() {
    printf("PID: %d\n", getpid());
    sleep(10);
    void *p1 = MyMalloc(100);
    void *p2 = MyMalloc(2048*1024);
    void *p3 = MyMalloc(300);
    printf("p1=%p, p2=%p, p3=%p\n", p1, p2, p3);
    sleep(15);
    MyFree(p2);
    sleep(15);
    MyFree(p1);
    MyFree(p3);
    sleep(15);
    munmap(heap_start, HEAP_SIZE);
    return 0;
}
