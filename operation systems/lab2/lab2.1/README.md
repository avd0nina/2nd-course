# Блок задач на “удовлетворительно”

+ Запустите программу `hello world` из предыдущей задачи под `strace`. Обратите внимание, какие системные вызовы были вызваны в процессе исполнения программы. Чем обусловлено такое количество системных вызовов. Какой системный вызов используется для вывода `hello world`? Изучите этот вызов и разберитесь, что он принимает и возвращает:

```bash
user@host:~$ gcc hello.c -o hello -Wall
user@host:~$ strace ./hello
```

+ Используйте этот сискол в программе `hello world` вместо `printf()`. Убедитесь, что этот вызов присутствует в выводе `strace`:

```bash
user@host:~$ gcc hello_write.c -o hello_write -Wall
user@host:~$ strace ./hello_write
```

+ Напишите свою обертку над этим сисколом. Для этого используйте функцию `syscall()` из `libc`. Также проверьте вывод
`strace`:

```bash
user@host:~$ gcc hello_syscall.c -o hello_syscall -Wall
user@host:~$ strace ./hello_syscall
```

+ Запустите под `strace` команду `wget kernel.org` (если нет `wget`, используйте `curl`). Получите статистику использования системных вызовов, порожденных процессом:

```bash
user@host:~$ strace -c wget kernel.org
```
