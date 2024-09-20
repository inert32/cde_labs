#ifndef __METHODS_H__
#define __METHODS_H__

#include <stddef.h>

class func_base {
public:
    virtual double calc(const double x) const = 0;
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

#endif /* __METHODS_H__ */
