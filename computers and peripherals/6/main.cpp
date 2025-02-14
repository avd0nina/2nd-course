#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdio.h>

void PrintDev(libusb_device* dev) {
    libusb_device_descriptor desc; // объявляем структуру desc, которая будет хранить описание устройства.
    int r = libusb_get_device_descriptor(dev, &desc); // вызываем функцию libusb_get_device_descriptor, которая заполняет desc информацией о переданном устройстве dev. Если функция возвращает отрицательное значение, это ошибка.
    if (r < 0) {
        std::cerr << "Ошибка: инициализация не выполнена, код: " << r << "\n";
        return;
    }
    printf("%d %d %d\n", (int)desc.bDeviceClass, desc.idVendor, desc.idProduct); // выводим класс устройства, идентификатор производителя и идентификатор устройства, которые содержатся в desc.
}

void PrintSerialNumber(libusb_device* dev) {
    libusb_device_descriptor desc; // libusb_device_descriptor desc; — создаем структуру для хранения описания устройства.
    libusb_device_handle* handle; // объявляем указатель на дескриптор устройства, который будет использоваться для открытия устройства.
    int r = libusb_get_device_descriptor(dev, &desc); // получаем дескриптор устройства.
    if (r < 0) {
        std::cerr << "Ошибка: инициализация не выполнена, код: " << r << "\п";
        return;
    }
    r = libusb_open(dev, &handle);
    if (r < 0) {
        std::cerr << "Ошибка: не удалось открыть интерфейс устройства: " << r << "\п";
    }
    unsigned char serial_number [30]; // создаем массив для хранения серийного номера устройства (до 30 символов).
    r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial_number, 30); // получаем серийный номер устройства в ASCII-формате.
    if (r < 0) {
        std::cerr << "Ошибка: не удалось получить серийный номер: " << r << "\n";
        return;
    }
    serial_number [29] = '\0'; // завершаем строку символом конца строки, чтобы корректно вывести её в консоль.
    std::cout << serial_number << "\n";
    libusb_close(handle);
}

int main() {
    libusb_device **devs; // объявляем указатель на список устройств.
    libusb_context *ctx = NULL; //  создаем контекст libusb для работы с библиотекой.
    int r;
    ssize_t cnt; // переменная для хранения количества найденных устройств.
    ssize_t i;
    r = libusb_init(&ctx); // инициализируем библиотеку libusb. Если возвращается отрицательное значение, выводим ошибку и завершаем программу.
    if (r < 0) {
        std::cerr << "Ошибка: инициализация не выполнена, код: " << r << "\n";
        return -1;
    }
    cnt = libusb_get_device_list(ctx, &devs); // получаем список подключенных USB-устройств.
    if (cnt < 0) {
        std::cerr << "Ошибка: список USB устройств не получен.\n";
        return -1;
    }
    printf("найдено устройств: %d\n", cnt);
    printf("=============================="
           "=============================\n");
    printf("| * класс устройства\n");
    printf("| | * идентификатор производителя\n");
    printf("| | | * идентификатор устройства\n");
    for (i = 0; i < cnt; i++) {
        PrintDev(devs[i]);
        printf("Серийный номер: ");
        PrintSerialNumber(devs[i]);
        printf("|==|===|===============================\n");
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return -1;
}
