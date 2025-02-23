# bayan


**BAYAN tool**
Утилита для поиска дубликатов файлов

Опции командной строки:

  -h [ --help ]                  Help screen


  -d [ --directories ] arg (=./) Directories to scan (may set several, space 
                                 separated)


  -e [ --except ] arg            Directories to skip during scan (may set 
                                 several, space separated)


  -r [ --recursive ] arg (=0)    Make recursive search (same for all 
                                 directories), false to stay in defined 
                                 directories only


  --min-file-size arg (=1)       Minimum file size in bytes


  -m [ --mask ] arg (=.*)        File name masks, allowed for comparison (case 
                                 insensitive)


  -b [ --block-size ] arg (=3)   Size of block during scan (bytes)


  --hash-algorithm arg (=crc32)  Hash algorithm to use (currently supported 
                                 crc32, md5)


  -p [ --print-params ] arg (=0) Display run parameters at startup



Пример сборки и запуска:

rm -R ./build

cmake -B build/

cmake --build ./build/

./build/bayan -d="/somepath1 /somepath2" -e="/some_excl_path1 /some_excl_path2" -r true -m .* --hash-algorithm md5


# Общая информация о задании:

Пользуясь имеющимися в библиотеке Boost структурами и алгоритмами
разработать утилиту для обнаружения файлов-дубликатов.
Утилита должна иметь возможность через параметры командной строки
указывать
• директории для сканирования (может быть несколько)
• директории для исключения из сканирования (может быть несколько)
• уровень сканирования (один на все директории, 0 - только указанная
директория без вложенных)
• минимальный размер файла, по умолчанию проверяются все файлы
больше 1 байта.
• маски имен файлов разрешенных для сравнения (не зависят от
регистра)
• размер блока, которым производится чтения файлов, в задании этот
размер упоминается как S
• один из имеющихся алгоритмов хэширования (crc32, md5 -
конкретные варианты определить самостоятельно), в задании
эта функция упоминается как H
Результатом работы утилиты должен быть список полных путей файлов
с идентичным содержимым, выводимый на стандартный вывод. На одной
строке один файл. Идентичные файлы должны подряд, одной группой.
Разные группы разделяются пустой строкой.
Обязательно свойство утилиты - бережное обращение с дисковым вводом
выводом. Каждый файл может быть представлен в виде списка блоков
размера S. Если размер файла не кратен, он дополняется бинарными
нулями.

Файлы считаются идентичными при полном совпадении последовательности
хешей блоков.
Самоконтроль
• блок файла читается с диска не более одного раза
• блок файла читается только в случае необходимости
• не забыть, что дубликатов может быть больше чем два
• пакет bayan содержащий исполняемый файл bayan опубликован на
bintray
• описание параметров в файле README.md корне репозитория
• отправлена на проверку ссылка на страницу репозитория