# Блок задач на “отлично”

+ Написать динамическую библиотеку с функцией `print_hello_from_dynamic_lib()` и использовать ее с `hello.c`:

```bash
user@host:~$ gcc hello_dynamic.c -c -fpic -o hello_dynamic.o -Wall
user@host:~$ gcc hello_dynamic.o -o libhello_dynamic.so -shared -Wall
user@host:~$ gcc hello.c -o hello -lhello_dynamic -L. -Wall
user@host:~$ LD_LIBRARY_PATH=./ ./hello
user@host:~$ export LD_LIBRARY_PATH=./
user@host:~$ ./hello
```

+ Посмотреть состояние функции `print_hello_from_dynamic_lib()` в получившимся исполняемом файле. Объяснить увиденное:
  
```bash
user@host:~$ nm ./hello
user@host:~$ ldd ./hello
```
