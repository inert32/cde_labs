// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "../build.h"
#include "../common/common.h"
#include "methods.h"

// Функции
class func1 : public func_base {
public:
    double calc(const double x) const {
        const double ret = 1.0 / x;
        if (std::isnan(ret) || std::isinf(ret)) return limit;
        else return ret;
    }
};
class func2 : public func_base {
public:
    double calc(const double x) const {
        const double ret = sin(x) / x;
        if (std::isnan(ret) || std::isinf(ret)) return limit;
        else return ret;
    }
    static constexpr double limit = 1.0;
};
class func3 : public func_base {
public:
    double calc(const double x) const {
        const double ret = 1.0 / (x * x + x + 1);
        if (std::isnan(ret) || std::isinf(ret)) return limit;
        else return ret;
    }
};
class func4 : public func_base {
public:
    double calc(const double x) const {
        const double ret = 1.0 / (x * x - x - 1);
        if (std::isnan(ret) || std::isinf(ret)) return limit;
        else return ret;
    }
};

func_base* func_select(const cli_map& cli) {
    try {
        switch (std::stoi(cli.at("func"))) {
            case 1: return new func1;
            case 2: return new func2;
            case 3: return new func3;
            case 4: [[fallthrough]];
            default: throw std::exception();
        }
    }
    catch (const std::exception&) {
        return new func4;
    }
}

int main(int argc, char** argv) {
    std::cout << "Lab1 " << build_version << " " << build_git << std::endl;

    // Разбор параметров
    const auto cli = parse_cli(argc, argv);
    size_t iter_count = sizet_from_cli_map(cli, "count", 1000);
    double from = double_from_cli_map(cli, "from", 0.0);
    double to = double_from_cli_map(cli, "to", 1.0);

    // Выбор функции
    func_base* fn; methods_base* method;
    fn = func_select(cli);
    method = method_select(cli);

    std::cout << method->calc(fn, iter_count, from, to) << std::endl;
    return 0;
}
