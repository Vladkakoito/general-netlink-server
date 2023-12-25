# Немного о проекте
* Сервер, принимающий сообщения(элементарные математические действия) в текстовом виде формата JSON по протоколу General Netlink (userspace), выполняющий действия из сообщения, и высылающий ответ.
* Клиент (в планах во множественном числе), который взаимодействует с этим сервером.
Используется библиотека libnl
## Demo: Для быстрого запуска - готовые бинарники с настройками в папке Demo. Не забывайте запускать под root-ом
## Для сборки требуется:
* компиляторы, расположенные в "/usr/bin/gcc" и "/usr/bin/g++". Ну или поменяйте переменные в CMake
* Использовался С++ 17
* Нужен cmake
## Процесс сборки. Из каталога проекта:
* cmake .
* cmake --build .

Бинарники окажутся в /bin.
Не забудьте файл конфига с указанным портом! (можно взять из папке Demo)

# Проект состоит
### Из сервера:
* слушает указанный порт
* принимает текстовые сообщения в формате JSON
* выполняет указанные действия с указанными аргументами
* отправляет тому кто выслал сообщение ответ в формате JSON

Все вышеописанные действия выполняются последовательно.
Прерывается по ctrl+c из терминала

### Из клиента:
* читает сообщения из Messages.json (при инициализации)
* отправляет их поочереди на сервер
* слушает ответ
* после выполнения всех запросов, записывает в файл результат в виде запрос - ответ

## Требования к Json сообщениям (пример в Messages.json в папке Demo. Должны лежать в каталоге с клиентом):
* root - массив сообщений
* сообщение это объект
* обязательно одно действие с ключом action. Поддерживаются значения: plus, minus, div, multi
* аргументы должны быть с ключами argument_[i]
* нумерация аргументов должна быть подряд. Можно начинать с нуля, или единицы
* количество аргументов для plus и multi от 1 до 100. Для minus и div сторого 2шт.

## Можно улучшить:
 1. ##### Логгер:
	* добавить уровни логгирования

2. ##### Клиент: 
	* добавить поддержку многопоточной работы
	* сделать кастомный парсинг JSON - что бы не разбирать JSON дальше root-массива

3. ##### Сервер:
	* добавить многопоточную обработку, что бы повысить предельную нагрузку
	* добавить поддержку вложенных действий, когда один оператор работает с результатом другого.
	* Расширить множество обрабатываемых действий
	* Добавить операторы приоритета: скобки
	* заменить библиотеку для работы с JSON - что бы объект был unordered_map вместо map, и что бы при парсинге данные не копировались
## ...
