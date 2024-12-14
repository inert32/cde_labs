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
    flatmat(const size_t Y, const size_t X) {
        size_x = X; size_y = Y;

        arr = new T*[size_y];
        for (size_t y = 0; y < size_y; y++) {
            arr[y] = new T[size_x];
            for (size_t x = 0; x < size_x; x++)
                arr[y][x] = (T)0;
        }

        //const size_t end = Y * X;
        //for (size_t i = 0; i < end; i++) arr[i] = (T)0;
    }
    size_t get_length(void) const { return size_x; }
    size_t get_height(void) const { return size_y; }

    T& operator()(const size_t y, const size_t x) {
        //return arr[y * size_y + x];
        return arr[y][x];
    }

private:
    T** arr = nullptr;
    size_t size_x = 0, size_y = 0;
};

#endif /* __FLATMAT_H__ */
