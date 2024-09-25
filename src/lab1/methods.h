#ifndef __METHODS_H__
#define __METHODS_H__

#include "../common.h"

class func_base {
public:
    virtual double calc(const double x) const = 0;
    static constexpr double limit = 0.0;
};

// Методы
class methods_base {
public:
    virtual double calc(const func_base* fn, const size_t count, const double from, const double to) const = 0;
};

// Метод Монте-Карло
class monte_carlo : public methods_base {
public:
    double calc(const func_base* fn, const size_t count, const double from, const double to) const;
};

// Метод прямоугольников
class quadrants : public methods_base {
public:
    double calc(const func_base* fn, const size_t count, const double from, const double to) const;
};

// Метод трапеций
class trapezoid : public methods_base {
public:
    double calc(const func_base* fn, const size_t count, const double from, const double to) const;
};

// Метод Симпсона
class simpson : public methods_base {
public:
    double calc(const func_base* fn, const size_t count, const double from, const double to) const;
};

// Выбор метода с помощью командной строки
methods_base* method_select(const cli_map& cli, const bool threads = false);

#endif /* __METHODS_H__ */
