# Лабораторные работы по численным методам
Лабораторные работы по численным методам решения уравнения переноса излучения

## Статус работ
* Численный расчет интеграла (Готово)
* Численное решение уравнения адвекции разностной схемой (Готово)
* Взаимодействие излучения с веществом (Запланировано)
* Решение уравнения переноса (Запланировано)

## Сборка
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
Для сборки проекта под Windows используется Visual Studio 2019 и выше. Для вывода графиков библиотеки SDL2 и SDL2-ttf можно установить с помощью vcpkg:

    vcpkg install sdl2 sdl2-ttf
