// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include "methods.h"

double monte_carlo::calc(const func_base* fn, const size_t count, const double from, const double to) const {
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

double quadrants::calc(const func_base* fn, const size_t count, const double from, const double to) const {
    std::cout << "Using quadrants method" << std::endl;

    const double step = (to - from) / (double)count;
    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        double y = fn->calc(from + step * i);
        if (std::isnan(y)) y = fn->limit;
        sum = sum + y;
    }

    return step * sum;
}

double trapezoid::calc(const func_base* fn, const size_t count, const double from, const double to) const {
    std::cout << "Using trapezoid method" << std::endl;

    const double step = (to - from) / (double)count;

    double y = fn->calc(from), borders = 0.0;
    if (std::isnan(y)) y = fn->limit;
    borders = y;

    y = fn->calc(to);
    if (std::isnan(y)) y = fn->limit;
    borders = 0.5 * (borders + y);

    double sum = 0.0;
    for (size_t i = 1; i < count - 1; i++) {
        double y = fn->calc(from + step * i);
        if (std::isnan(y)) y = fn->limit;
        sum = sum + y;
    }

    return step * (sum + borders);
}

double simpson::calc(const func_base* fn, const size_t count, const double from, const double to) const {
    std::cout << "Using Simpson's method" << std::endl;

    const size_t count_n = count * 2;
    const double step = (to - from) / count_n;

    double sum1 = 0.0, sum2 = 0.0;
    for (size_t i = 1; i < count_n; i+=2) {
        double y = fn->calc(from + step * i);
        if (std::isnan(y)) y = fn->limit;

        sum1 = sum1 + y; // Проход по нечетным членам

        y = fn->calc(from + step * (i + 1));
        if (std::isnan(y)) y = fn->limit;
        sum2 = sum2 + y; // Проход по четным членам
    }

    sum1 = sum1 * 4.0;
    sum2 = sum2 * 2.0;

    double _from = fn->calc(from);
    if (std::isnan(_from)) _from = fn->limit;
    double _to = fn->calc(from);
    if (std::isnan(_to)) _to = fn->limit;

    return step / 3.0 * (_from + _to + sum1 + sum2);
}

methods_base* method_select(const cli_map& cli, [[maybe_unused]] const bool threads) {
    try {
        const auto name = cli.at("method");

        if (name == "quad") return new quadrants;
        else if (name == "trapez") return new trapezoid;
        else if (name == "simpson") return new simpson;
        else throw std::exception();
    }
    catch (const std::exception&) {
        return new monte_carlo;
    }
}
