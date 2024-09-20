// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "../build.h"
#include "../common.h"

// Функции
class functions {
public:
    virtual double calc(const double x) const = 0;
};
class func1 : public functions {
public:
    double calc(const double x) const {
        return 1.0 / x;
    }
};
class func2 : public functions {
public:
    double calc(const double x) const {
        return sin(x) / x;
    }
};
class func3 : public functions {
public:
    double calc(const double x) const {
        return 1.0 / (x * x + x + 1);
    }
};
class func4 : public functions {
public:
    double calc(const double x) const {
        return 1.0 / (x * x - x - 1);
    }
};

// Методы
class methods_base {
public:
    virtual double calc(const functions* fn, const size_t count, const double from, const double to) const = 0;
};

// Метод Монте-Карло
class monte_carlo : public methods_base {
public:
    double calc(const functions* fn, const size_t count, const double from, const double to) const {
        std::cout << "Using Monte-Carlo method" << std::endl;

        // Запуск генератора
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution nums(from, to);

        // Расчет попаданий
        double sum = 0.0;
        for (size_t i = 0; i < count; i++)
            sum = sum + fn->calc(nums(gen));

        const double mult = (to - from) / (double)count;

        return sum * mult;
    }
};

// Метод прямоугольников
class quadrants : public methods_base {
public:
    double calc(const functions* fn, const size_t count, const double from, const double to) const {
        std::cout << "Using quadrants method" << std::endl;

        const double step = (to - from) / (double)count;
        double sum = 0.0;
        for (size_t i = 0; i < count; i++)
            sum = sum + fn->calc(step * i);

        return step * sum;
    }
};

int main(int argc, char** argv) {
    std::cout << "Lab1 " << build_version << " " << build_git << std::endl;

    // Разбор параметров
    size_t iter_count = 1000;
    double from = 0, to = 1;
    functions* fn; methods_base* method;

    const auto cli = parse_cli(argc, argv);
    auto it = cli.find("count");
    if (it != cli.end()) iter_count = std::stoi(it->second);
    it = cli.find("from");
    if (it != cli.end()) from = std::stod(it->second);
    it = cli.find("to");
    if (it != cli.end()) to = std::stod(it->second);

    // Выбор функции
    it = cli.find("func");
    if (it != cli.end()) {
        switch (std::stoi(it->second)) {
            case 1: fn = new func1; break;
            case 2: fn = new func2; break;
            case 3: fn = new func3; break;
            case 4:
            default: fn = new func4; break;
        }
    } else fn = new func4;

    // Выбор метода
    it = cli.find("method");
    if (it != cli.end()) {
        if (it->second == "mt") method = new monte_carlo;
        else if (it->second == "quad") method = new quadrants;
        else method = new monte_carlo;
    } else method = new monte_carlo;

    std::cout << method->calc(fn, iter_count, from, to) << std::endl;
    return 0;
}
