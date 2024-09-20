// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "../build.h"

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
// Метод Монте-Карло
double monte_carlo(const functions* fn, const size_t count, const double from, const double to) {
    std::cout << "Using Monte-Carlo method" << std::endl;

    // Запуск генератора
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums(from, to);

    // Расчет попаданий
    double sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum = sum + fn->calc(nums(gen));
    }

    const double mult = (to - from) / (double)count;

    return sum * mult;
}

int main() {
    std::cout << "Lab1 " << build_version << " " << build_git << std::endl;

    size_t iter_count = 1000;
    double from = 0, to = 1;

    functions* fn = new func3;
    std::cout << monte_carlo(fn, iter_count, from, to) << std::endl;
    return 0;
}
