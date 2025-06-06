# Понятие пользователя. Управление правами

## Использование SUID-бита для доступа к файлам

1. Создайте файл с правами “чтение только владельцу”.
2. Напишите программу, которая выводит:
   + содержимое этого файла.
   + реальный и эффективный идентификаторы пользователя
3. Запустите программу из-под своего пользователя и из-под чужого.
4. Объясните результат.
5. Установите SUID-бит.
6. Запустите программу из-под своего пользователя и из-под чужого.
7. Объясните результат.


1. Создание файлов
```bash
nano reader.c
```

```bash
echo "secret" > secret.txt
```

2. Компиляция
```bash
gcc reader.c -o reader
```

3. Изменение прав доступа
```bash
chmod 400 secret.txt
```

4. Проверка прав
```bash
ls -l
```

5. Запуск без SUID-бита с администратора
```bash
./reader
```

6. Запуск без SUID-бита с другого пользователя
```bash
sudo -u user ./reader
```

7. Установка SUID-бита
```bash
chmod u+s reader
```

8. Проверка прав
```bash
ls -l
```

9. Запуск c SUID-битом с администратора
```bash
./reader
```

10. Запуск с SUID-битом с другого пользователя
```bash
sudo -u user ./reader
```







