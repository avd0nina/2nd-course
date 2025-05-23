#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define LINE_LEN 512

void PrintPagemapEntry(uint64_t entry, uintptr_t vaddr) {
    printf("Виртуальный адрес: 0x%lx\n", vaddr);
    printf("raw entry: 0x%016lx\n", entry);
    if (entry & (1ULL << 63)) { // проверяет, установлен ли 63-й бит в entry (бит present). бит 63 = 1 означает, что страница находится в физической памяти (RAM)
        printf("Страница в памяти\n");
        uint64_t pfn = entry & ((1ULL << 55) - 1); // извлекает биты 0–54 из entry, которые содержат Page Frame Number. PFN — это номер физической страницы в RAM
        printf("PFN: 0x%lx\n", pfn);
    } else {
        printf("Страница НЕ в памяти\n");
    }
    if (entry & (1ULL << 62)) { // проверяет, установлен ли 62-й бит в entry (бит swapped)
        printf("Страница в swap\n");
    }
    printf("\n");
}

uint64_t ReadPagemapEntry(int fd, uintptr_t vaddr, size_t page_size) {
    uint64_t offset = (vaddr / page_size) * 8; // смещение в файле
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) { // переходит к записи, соответствующей vaddr
        perror("Ошибка lseek");
        return 0;
    }
    uint64_t entry;
    if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) { // читает 8 байт из файла в переменную entry
        perror("Ошибка чтения записи");
        return 0;
    }
    return entry;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }
    pid_t pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Неверный PID\n");
        return 1;
    }
    char maps_path[256], pagemap_path[256];
    snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
    snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%d/pagemap", pid);
    FILE *maps = fopen(maps_path, "r");
    if (!maps) {
        perror("Не удалось открыть maps");
        return 1;
    }
    int pagemap_fd = open(pagemap_path, O_RDONLY);
    if (pagemap_fd < 0) {
        perror("Не удалось открыть pagemap");
        fclose(maps);
        return 1;
    }
    size_t page_size = getpagesize(); // размер страницы памяти
    char line[LINE_LEN]; // буфер для чтения строк
    while (fgets(line, sizeof(line), maps)) { // читаем строки из /proc/<PID>/maps
        uintptr_t start, end; // начальный и конечный адреса регионов
        char perms[5], path[LINE_LEN]; // права доступа и путь к файлу
        if (sscanf(line, "%lx-%lx %4s %*s %*s %*s %[^\n]", &start, &end, perms, path) < 3) { // извлекает данные о регионе памяти (адреса и права)
            continue;
        }
        if (strchr(perms, 'r') && perms[3] == 'p') { // проверяет, имеет ли регион права на чтение и является ли приватным
            uintptr_t vaddr = start;
            uint64_t entry = ReadPagemapEntry(pagemap_fd, vaddr, page_size);
            PrintPagemapEntry(entry, vaddr);
        }
    }
    fclose(maps);
    close(pagemap_fd);
    return 0;
}
