# Задание: IP и ICMP в Wireshark
## Цели задания
1. Практическое изучение протоколов уровня IP и ICMP
2. Знакомство с утилитами ping и traceroute (tracert)
### Вам понадобится
Traceroute - на Linux может потребоваться установка, на Windows предустановлена.
## Задачи
1. Запустить Wireshark, начать захват пакетов с вашего основного сетевого интерфейса.
2. В строке фильтра ввести “icmp”.
3. Открыть командную строку/терминал, выполнить команду “ping ccfit.nsu.ru”. 
При успешном выполнении команды будет сделан ICMP-запрос и получен ответ от сервера. Если запросы отправляются бесконечно, можно вручную завершить выполнение (control + C).
4. Изучить консольный вывод.
5. Изучить IP- и ICMP-заголовки захваченных ICMP-пакетов в Wireshark.
6. Выполнить команду “traceroute ccfit.nsu.ru” (tracert на Windows). Может потребоваться подождать некоторое время.
При успешном выполнении команды вы увидите в консоли путь, по которому пакеты добираются до назначения (хоста с доменным именем ccfit.nsu.ru).
7. Изучите ICMP-пакеты, захваченные в Wireshark. На этом примере понять, как traceroute получает информацию о пути, используя механизмы протокола ICMP.


