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
        sum = sum + fn->calc(step * i);

    return step * sum;
}

methods_base* method_select([[maybe_unused]] const bool threads) {
    return new monte_carlo;
}
methods_base* method_select(const cli_map& cli, [[maybe_unused]] const bool threads) {
    try {
        const auto name = cli.at("method");

        if (name == "quad") return new quadrants;
        else throw std::exception();
    }
    catch (const std::exception&) {
        return new monte_carlo;
    }
}
