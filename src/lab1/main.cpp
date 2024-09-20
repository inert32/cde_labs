// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "../build.h"
#include "../common.h"
#include "methods.h"

// Функции
class func_base;
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
    method = (it != cli.end()) ? method_select(it->second) : method_select();

    std::cout << method->calc(fn, iter_count, from, to) << std::endl;
    return 0;
}
