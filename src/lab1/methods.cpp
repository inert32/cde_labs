/*
Laboratory work on numerical methods for solving convection-diffusion equations
Copyright (C) 2024 Mikhaylov Alexey (inert32@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
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
    for (size_t i = 0; i < count; i++)
        sum = sum + fn->calc(from + step * (double)i);

    return step * sum;
}

double trapezoid::calc(const func_base* fn, const size_t count, const double from, const double to) const {
    std::cout << "Using trapezoid method" << std::endl;

    const double step = (to - from) / (double)count;
    const double borders = 0.5 * (fn->calc(from) + fn->calc(to));

    double sum = 0.0;
    for (size_t i = 1; i < count - 1; i++)
        sum = sum + fn->calc(from + step * (double)i);

    return step * (sum + borders);
}

double simpson::calc(const func_base* fn, const size_t count, const double from, const double to) const {
    std::cout << "Using Simpson's method" << std::endl;

    const size_t count_n = count * 2;
    const double step = (to - from) / (double)count_n;

    double sum1 = 0.0, sum2 = 0.0;
    for (size_t i = 1; i < count_n; i+=2) {
        sum1 = sum1 + fn->calc(from + step * (double)i); // Проход по нечетным членам
        sum2 = sum2 + fn->calc(from + step * (double)(i + 1)); // Проход по четным членам
    }

    sum1 = sum1 * 4.0;
    sum2 = sum2 * 2.0;

    return step / 3.0 * (fn->calc(from) + fn->calc(to) + sum1 + sum2);
}

