#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void CreateDirectory(const char *path) { // Функция для создания каталога
    if (mkdir(path, 0755) == -1) { // Создаем каталог с правами доступа 0755 (rwxr-xr-x)
        perror("Ошибка при создании каталога");
    } else {
        printf("Каталог '%s' успешно создан.\n", path);
    }
}

void ListDirectory(const char *path) { // Функция для вывода содержимого каталога
    DIR *dir = opendir(path); // Открываем каталог
    if (dir == NULL) {
        perror("Ошибка при открытии каталога");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) { // Читаем каждый элемент каталога
        printf("%s\n", entry->d_name); // Выводим имя элемента
    }
    closedir(dir);
}

void RemoveDirectory(const char *path) { // Функция для удаления каталога
    if (rmdir(path) == -1) { // Удаляем каталог
        perror("Ошибка при удалении каталога");
    } else {
        printf("Каталог '%s' успешно удален.\n", path);
    }
}

void CreateFile(const char *path) { // Функция для создания файла
    int fd = open(path, O_CREAT | O_WRONLY, 0644); // Создаем файл с правами доступа 0644 (rw-r--r--)
    if (fd == -1) {
        perror("Ошибка при создании файла");
    } else {
        close(fd); // Закрываем файл
        printf("Файл '%s' успешно создан.\n", path);
    }
}

void ReadFile(const char *path) { // Функция для вывода содержимого файла
    char buffer[1024];
    int fd = open(path, O_RDONLY); // Открываем файл для чтения
    if (fd == -1) {
        perror("Ошибка при открытии файла"); // Выводим сообщение об ошибке, если файл не удалось открыть
        return;
    }
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer)))) { // Читаем файл по частям
        if (bytes_read == -1) {
            perror("Ошибка при чтении файла"); // Выводим сообщение об ошибке, если чтение не удалось
            break;
        }
        write(STDOUT_FILENO, buffer, bytes_read); // Выводим содержимое файла на экран
    }
    close(fd); // Закрываем файл
}
    
void RemoveFile(const char *path) { // Функция для удаления файла
    if (unlink(path) == -1) { // Удаляем файл
        perror("Ошибка при удалении файла"); // Выводим сообщение об ошибке, если что-то пошло не так
    } else {
        printf("Файл '%s' успешно удален.\n", path); // Сообщаем об успешном удалении файла
    }
}

void CreateSymlink(const char *target, const char *link_path) { // Функция для создания символьной ссылки
    if (symlink(target, link_path) == -1) { // Создаем символьную ссылку
        perror("Ошибка при создании символьной ссылки"); // Выводим сообщение об ошибке, если что-то пошло не так
    } else {
        printf("Символьная ссылка '%s' -> '%s' успешно создана.\n", link_path, target); // Сообщаем об успешном создании ссылки
    }
}
    
void ReadSymlink(const char *path) { // Функция для вывода содержимого символьной ссылки
    char buffer[1024];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1); // Читаем содержимое символьной ссылки
    if (len == -1) {
        perror("Ошибка при чтении символьной ссылки"); // Выводим сообщение об ошибке, если чтение не удалось
    } else {
        buffer[len] = '\0'; // Добавляем завершающий нулевой символ
        printf("Символьная ссылка '%s' указывает на: %s\n", path, buffer); // Выводим содержимое ссылки
    }
}
        
void ReadSymlinkTarget(const char *path) { // Функция для вывода содержимого файла, на который указывает символьная ссылка
    char buffer[1024];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1); // Читаем содержимое символьной ссылки
    if (len == -1) {
        perror("Ошибка при чтении символьной ссылки"); // Выводим сообщение об ошибке, если чтение не удалось
        return;
    }
    buffer[len] = '\0'; // Добавляем завершающий нулевой символ
    ReadFile(buffer); // Выводим содержимое файла, на который указывает ссылка
}
        
void RemoveSymlink(const char *path) { // Функция для удаления символьной ссылки
    if (unlink(path) == -1) { // Удаляем символьную ссылку
        perror("Ошибка при удалении символьной ссылки"); // Выводим сообщение об ошибке, если что-то пошло не так
    } else {
        printf("Символьная ссылка '%s' успешно удалена.\n", path); // Сообщаем об успешном удалении ссылки
    }
}
        
void CreateHardlink(const char *target, const char *link_path) { // Функция для создания жесткой ссылки
    if (link(target, link_path) == -1) { // Создаем жесткую ссылку
        perror("Ошибка при создании жесткой ссылки");
    } else {
        printf("Жесткая ссылка '%s' -> '%s' успешно создана.\n", link_path, target);
    }
}
        
void RemoveHardlink(const char *path) { // Функция для удаления жесткой ссылки
    if (unlink(path) == -1) { // Удаляем жесткую ссылку
        perror("Ошибка при удалении жесткой ссылки"); // Выводим сообщение об ошибке, если что-то пошло не так
    } else {
        printf("Жесткая ссылка '%s' успешно удалена.\n", path); // Сообщаем об успешном удалении ссылки
    }
}
        
void PrintFileInfo(const char *path) { // Функция для вывода прав доступа и количества жестких ссылок
    struct stat st;
    if (stat(path, &st) == -1) { // Получаем информацию о файле
        perror("Ошибка при получении информации о файле");
        return;
    }
    printf("Права доступа: %o\n", st.st_mode & 0777); // Выводим права доступа
    printf("Количество жестких ссылок: %lu\n", st.st_nlink); // Выводим количество жестких ссылок
}
            
void ChangePermissions(const char *path, mode_t mode) { // Функция для изменения прав доступа
    if (chmod(path, mode) == -1) { // Изменяем права доступа
        perror("Ошибка при изменении прав доступа");
    } else {
        printf("Права доступа к файлу '%s' успешно изменены.\n", path);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <аргументы>\n", argv[0]); // Выводим сообщение об ошибке, если аргументов недостаточно
        return 1;
    }
    // Определяем действие на основе имени программы
    const char *action = strrchr(argv[0], '/'); // Ищем последний символ '/' в имени программы
    action = action ? action + 1 : argv[0]; // Получаем имя программы без пути
    if (strcmp(action, "create_dir") == 0) {
        CreateDirectory(argv[1]); // Создаем каталог
    } else if (strcmp(action, "list_dir") == 0) {
        ListDirectory(argv[1]); // Выводим содержимое каталога
    } else if (strcmp(action, "remove_dir") == 0) {
        RemoveDirectory(argv[1]); // Удаляем каталог
    } else if (strcmp(action, "create_file") == 0) {
        CreateFile(argv[1]); // Создаем файл
    } else if (strcmp(action, "read_file") == 0) {
        ReadFile(argv[1]); // Читаем файл
    } else if (strcmp(action, "remove_file") == 0) {
        RemoveFile(argv[1]); // Удаляем файл
    } else if (strcmp(action, "create_symlink") == 0) {
        CreateSymlink(argv[1], argv[2]); // Создаем символьную ссылку
    } else if (strcmp(action, "read_symlink") == 0) {
        ReadSymlink(argv[1]); // Читаем символьную ссылку
    } else if (strcmp(action, "read_symlink_target") == 0) {
        ReadSymlinkTarget(argv[1]); // Читаем файл, на который указывает символьная ссылка
    } else if (strcmp(action, "remove_symlink") == 0) {
        RemoveSymlink(argv[1]); // Удаляем символьную ссылку
    } else if (strcmp(action, "create_hardlink") == 0) {
        CreateHardlink(argv[1], argv[2]); // Создаем жесткую ссылку
    } else if (strcmp(action, "remove_hardlink") == 0) {
        RemoveHardlink(argv[1]); // Удаляем жесткую ссылку
    } else if (strcmp(action, "print_file_info") == 0) {
        PrintFileInfo(argv[1]); // Выводим информацию о файле
    } else if (strcmp(action, "change_permissions") == 0) {
        ChangePermissions(argv[1], strtol(argv[2], NULL, 8)); // Изменяем права доступа
    } else {
        fprintf(stderr, "Неизвестное действие: %s\n", action); // Выводим сообщение об ошибке, если действие неизвестно
        return 1;
    }
    return 0;
}
                
