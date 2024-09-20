// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "../build.h"
#include "../common.h"
#include "methods.h"

// Функции
class func1 : public func_base {
public:
    double calc(const double x) const {
        return 1.0 / x;
    }
};
class func2 : public func_base {
public:
    double calc(const double x) const {
        return sin(x) / x;
    }
};
class func3 : public func_base {
public:
    double calc(const double x) const {
        return 1.0 / (x * x + x + 1);
    }
};
class func4 : public func_base {
public:
    double calc(const double x) const {
        return 1.0 / (x * x - x - 1);
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
    size_t iter_count = 1000;
    double from = 0, to = 1;
    func_base* fn; methods_base* method;

    const auto cli = parse_cli(argc, argv);
    auto it = cli.find("count");
    if (it != cli.end()) iter_count = std::stoi(it->second);
    it = cli.find("from");
    if (it != cli.end()) from = std::stod(it->second);
    it = cli.find("to");
    if (it != cli.end()) to = std::stod(it->second);

    // Выбор функции
    fn = func_select(cli);
    method = method_select(cli);

    std::cout << method->calc(fn, iter_count, from, to) << std::endl;
    return 0;
}
