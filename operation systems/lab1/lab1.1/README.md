# Блок задач на “удовлетворительно”

+ Написать программу `hello.c`, которая выводит фразу `“Hello world”`;
a. Получить исполняемый файл:

```bash
user@host:~$ gcc hello.c -o hello -Wall
```

b. Посмотреть `unresolved symbols` (`puts`, `printf`) с помощью `nm`:

```bash
user@host:~$ nm ./hello 
```

c. Посмотреть зависимости разделяемых библиотек:

```bash
user@host:~$ otool -L ./hello 
```

d Запустить:

```bash
user@host:~$ ./hello 
```
