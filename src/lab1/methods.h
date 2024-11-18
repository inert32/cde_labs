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
#ifndef __METHODS_H__
#define __METHODS_H__

class func_base {
public:
    virtual ~func_base() = default;
    virtual double calc(const double x) const = 0;
    static constexpr double limit = 0.0;
};

// Методы
class methods_base {
public:
    virtual ~methods_base() = default;
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

#endif /* __METHODS_H__ */
