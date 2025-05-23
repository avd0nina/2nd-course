#!/bin/bash
MAX_CLIENTS=1024  # Не больше, чем может обработать select()
DELAY=0.01        # Задержка между запусками (секунды)

for ((i=1; i<=MAX_CLIENTS; i++)); do
    ( echo "Client $i"; sleep 60 ) | nc -w 5 127.0.0.1 12345 >/dev/null &
    sleep "$DELAY"  # Даём серверу время обработать соединение
    echo "Запущен клиент $i"
done

wait
echo "Тест завершён"
