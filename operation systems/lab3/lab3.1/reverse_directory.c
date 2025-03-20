#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void ReverseString(char *string) {
    int length = strlen(string);
    for (int i = 0; i < length / 2; i++) {
        char temp = string[i];
        string[i] = string[length - i - 1]; //oello
        string[length - i - 1] = temp;//oellh
    } //olleh
}

void ReverseFileContent(const char *src_path, const char *dst_path) {
    int src_fd = open(src_path, O_RDONLY); //только читаю
    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644); // rw- r-- r--
    off_t size_file = lseek(src_fd, 0, SEEK_END); // узнаю размер файла
    char buffer;
    for (off_t i = size_file - 1; i >= 0; i--) {
        lseek(src_fd, i, SEEK_SET);
        read(src_fd, &buffer, 1);
        write(dst_fd, &buffer, 1);
    }
    close(src_fd);
    close(dst_fd);
}

void CopyReverseCatalog(const char* src_dir) {
    char dst_dir[256];
    if (strlen(src_dir) >= sizeof(dst_dir)) {
        fprintf(stderr, "Ошибка: путь к каталогу слишком длинный\n");
        return;
    }
    strcpy(dst_dir, src_dir); // Копируем путь к каталогу
    ReverseString(dst_dir); // Переворачиваем путь
    if (mkdir(dst_dir, 0755) == -1 && errno != EEXIST) { // создаем каталог
        perror("Ошибка при создании каталога");
        return;
    }
    DIR *dir = opendir(src_dir); // открываем каталог
    if (!dir) {
        perror("Ошибка при открытии каталога");
        return;
    }
    struct dirent *entry; // чтение записей из каталога. d_name — строка, содержащая имя файла или подкаталога (например, file.txt или dir). d_type — тип записи (например, обычный файл, подкаталог, символическая ссылка и т.д.).
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // проверка на регулярный файл
            char src_file_path[512];
            char dst_file_path[512];
            snprintf(src_file_path, sizeof(src_file_path), "%s/%s", src_dir, entry->d_name); // src_dir = "/home/user/documents", entry->d_name = "file.txt", src_file_path = "/home/user/documents/file.txt"
            snprintf(dst_file_path, sizeof(dst_file_path), "%s/", dst_dir); // dst_dir = "/stnemucod/resu/emoh", dst_file_path = "/stnemucod/resu/emoh"
            ReverseString(entry->d_name); // entry->d_name = "file.txt", после вызова ReverseString имя файла станет "txet.elif"
            strncat(dst_file_path, entry->d_name, sizeof(dst_file_path) - strlen(dst_file_path) - 1); // dst_file_path = "/home/user/backup/txet.elif"
            ReverseFileContent(src_file_path, dst_file_path);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    CopyReverseCatalog(argv[1]);
    return 0;
}
