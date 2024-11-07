# Лабораторные работы по численным методам

Лабораторные работы по численным методам решения уравнения переноса излучения

## Статус работ

* Численный расчет интеграла (Готово).
* Численное решение уравнения адвекции разностной схемой (Готово).
* Взаимодействие излучения с веществом (Готово).
* Решение уравнения переноса (Запланировано).

## Сборка

Требования для сборки проекта:
* Компилятор C++17 (Visual Studio 2017 или GCC),
* CMake для создания проекта.

Дополнительно SDL2 для вывода графики (lab2 и lab3).

### Debian/Ubuntu

Для минимальной сборки требуются только CMake и компилятор C++17:

    $ sudo apt install build-essential cmake git
    $ git clone https://github.com/inert32/cde_labs.git
    $ cd cde_labs
    $ mkdir build
    $ cmake ..
    $ make

Для вывода графиков (версия >=0.2.1) используется библиотека SDL2:

    $ sudo apt install libsdl2-dev libsdl2-ttf-dev
    $ cmake .. -DENABLE_SDL=on
    $ make

### Windows

Для сборки проекта под Windows используется Visual Studio 2017 и выше.
Для вывода графиков библиотеки SDL2 и SDL2-ttf можно установить с помощью vcpkg:

    vcpkg install sdl2 sdl2-ttf

Либо в корне проекта в каталог /3rdparty положить devel-версии библиотек.

## Запуск

Все проекты используют командную строку для задания параметров работы. Параметры задаются в виде "ключ=значение".
Кроме этого, для работы SDL2-ttf требуется любой файл TrueType шрифтов в рабочем каталоге с названием "font.ttf"

### Lab1

Параметры:
* iter_count - Число итераций (для метода Монте-Карло) или число отрезков,
* from - Левый конец отрезка интегрирования,
* to - Правый конец отрезка интегрирования,
* func - Выбор функции для интегрирования. В данный момент доступны четыре функции, под номерами 1,2,3,4.
* method - Выбор метода интегрирования. Варианты:
    * quad - Метод прямоугольников,
    * trapez - Метод трапеций,
    * simpson - Метод Симпсона,
    * Любой другой параметр или его отсутствие - Метод Монте-Карло.

### Lab2

Параметры:
* x_start - Начальная точка переноса,
* x_end - Конечная точка переноса,
* t_end - Время завершения переноса,
* x_step - Шаг по оси OX,
* t_step - Шаг по оси OT,
* speed - Скорость переноса,
* output - Путь до выходного файла,
* graph - Вывод графика на экран. Требует SDL2.

### Lab3

Кроме параметров командной строки, Lab3 требует файл задания.
Параметры:
* file - Путь до файла задания (обязательный параметр),
* graph - Вывод трассеров на экран,
* stat - Вывод статистики по частицам.

Формат файла заданий:
* area - Задать область симуляции  
  area <длина> <высота>

* subarea - Задать подобласть с веществом (material) внутри главной области  
  subarea <начальная координата X> <длина> <вещество>

* source - Задать источник излучения  
  source <тип> <координата X> <координата Y> <угол отклонения от 90 градусов>  
  source point - Точечный источник

* energy - Задать распределение энергий  
  Используются пары <энергия, МэВ = вероятность>  
  energy <пара> <пара> ...

* material - Задать параметры вещества  
  material <название> <сечение взаимодействия> <вероятность поглощения>
