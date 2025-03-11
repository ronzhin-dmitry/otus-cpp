# Модифицированный асинхронный парсер команд

Тестировать из консоли можно такой командой:

```
# Запуск 10 клиентов с обычными командами
seq 10 | xargs -I{} -P10 bash -c 'for i in {1..10}; do echo "client{}_cmd$i"; done | nc localhost 9000' &

# Запуск 3 клиентов с блоками
seq 3 | xargs -I{} -P3 bash -c 'echo -e "{\n1\n2\n3\n4\n5\n6\n7\n}\n" | nc localhost 9000' &

# Ждать завершения всех процессов
wait
```


# 10. Асинхронная сетевая обработка команд

Цель – переработать Задание 9 так, чтобы ввод данных осуществлялся по сети.

Необходимо разработать асинхронный сервер, работающий с протоколом, аналогичным консоли. Сервер принимает команды построчно.

Дальнейшая обработка остаётся без изменений – вывод в консоль и файлы, статический блоки и блоки динамического размера.

В качестве библиотеки сетевого взаимодействия следует использовать библиотеку Boost.ASIO (используем асинхронные возможности).

Поскольку не предполагается отправка данных от сервера клиенту в ответ на команды, основное взаимодействие будет вокруг методов async_read и async_accept.

Порядок запуска:

# bulk_server <port> <bulk_size>
где

port – номер tcp порта для входящих соединений. Соединения должны обслуживаться стпроизвольных интерфейсов

bulk_size – уже знакомый размер блока команд.

Не должно быть искусственных ограничений на количество одновременно подключенных клиентов. Команды из статических блоков, полученные из разных соединений, должны смешиваться между собой. Однако команды из динамических блоков (ограниченные символами { и } ) из разных соединений смешиваться не должны.

Пример запуска команд:

bulk_server 9000 3
и

seq 0 9 | nc localhost 9000
(что означает формирование 10 строк с числами от 0 до 9 на каждой строке и отправкой из по сетина локальный порт 9000).


Запуск команд должен привести к появлению в консоли:
bulk: 0, 1, 2
bulk: 3, 4, 5
bulk: 6, 7, 8
bulk: 9
и формированию четырёх файлов с содержимым, аналогичным каждой строке.
Одновременный запуск двух команд (например, из двух разных консолей) при уже ранее запущенном сервере с bulk_size == 3:

seq 0 9 | nc localhost 9000
и

seq 10 19 | nc localhost 9000
(что означает формирование 20 строк с числами от 0 до 19 на каждой строке и отправкой их по сети на локальный порт 9000 из разных соединений) должны привести к появлению в консоли:


bulk: 0, 1, 10
bulk: 2, 3, 11
bulk: 4, 5, 12
bulk: 6, 7, 13
bulk: 8, 9, 14
bulk: 15, 16, 17
bulk: 18, 19
и формированию семи файлов с содержимым, аналогичным каждой строке.

При этом - как именно смешаются команды из разных источников зависит от условий времени исполнения и может отличаться от запуска к запуску.



# Требования к реализации
Результатом работы должен стать исполняемый файл bulk_server, который находится в пакете bulk_server. Специальных усилий по демонизации сервера выполнять не стоит – сразу после запуска сервер может не возвращать управление вплоть до принудительного завершения. Результат работы должен быть опубликован на bintray.



# Проверка
Задание считается выполненным успешно, если после установки пакета и запуска с тестовыми данными вывод соответствует описанию Задания 9. Будет отмечена способность не терять команды, полученные непосредственно перед закрытием соединения клиентом.