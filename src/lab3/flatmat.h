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
#ifndef __FLATMAT_H__
#define __FLATMAT_H__

#include <memory>

/* Матрица Y*X, в виде непрерывного массива */
template <class T>
class flatmat {
public:
    flatmat(const size_t X, const size_t Y) :
    arr{new T[Y * X]} {
        size_x = X; size_y = Y;

        const size_t end = Y * X;
        // shared_ptr имеет оператор [], который переведет size_t в ptrdiff_t.
        // Это вызывает предупреждение -Wsign-conversion, поэтому берем указатель
        // на массив самостоятельно и обнуляем элементы. То же для flatmat::operator().
        auto ptr = arr.get();
        for (size_t i = 0; i < end; i++) ptr[i] = (T)0;
    }
    size_t get_length(void) const { return size_x; }
    size_t get_height(void) const { return size_y; }

    T& operator()(const size_t y, const size_t x) { return arr.get()[y * size_y + x]; }

private:
    std::shared_ptr<T[]> arr;
    size_t size_x = 0, size_y = 0;
};

#endif /* __FLATMAT_H__ */
