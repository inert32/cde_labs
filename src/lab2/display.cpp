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

#ifdef __ENABLE_GRAPH__

#include <iostream>
#include <stdexcept>
#include "display.h"

sdl_display::sdl_display() : sdl_display_base("Lab2") {
    // Создание координатной сетки
    coord_grid = new sdl_grid(10, 10, { area_x_start , area_y_start, area_x_end - area_x_start, area_y_end - area_y_start }, text);
}

sdl_display::~sdl_display() {
    delete coord_grid;
}

void sdl_display::show_frame(const mesh_t& mesh, const size_t curr, const double t_step) {
    clear_screen();

    // Рисуем границы графика
    SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
    SDL_FPoint area[6] = { {area_x_start, area_y_start}, {area_x_start, area_y_end}, {area_x_end, area_y_end}, {area_x_end, area_y_start}, {area_x_start, area_y_start}, {area_x_start, area_y_end}, };
    if (SDL_RenderDrawLinesF(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    // Рисуем координатную сетку
    coord_grid->draw_axes(rend, mesh, curr);

    // Рисуем график
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    const auto points = scale_graph(mesh, curr);
    if (SDL_RenderDrawLinesF(rend, points, (int)mesh.get_size().x) < 0) throw std::runtime_error(SDL_GetError());
    delete[] points;

    // Вывести текущее время
    auto time = text->cut_number((double)curr * t_step, 3);
    text->render_text("Time: " + time + "s", (int)area_x_start, (int)(0.91f * (float)len_y));

    // Передаем на экран
    SDL_RenderPresent(rend);
}

SDL_FPoint* sdl_display::scale_graph(const mesh_t& mesh, const size_t curr) {
    // Создаем точки
    auto size = mesh.get_size().x;
    SDL_FPoint* points = new SDL_FPoint[size];

    auto abs = mesh.get_layer_x();
    auto layer = mesh.get_layer(curr);

    // Пределы для оси OX
    auto x_min = abs[0];
    auto delta_x = abs[size - 1] - x_min;

    // Пределы для оси OY
    auto y_min = mesh.get_min_on_layer(curr);
    auto y_max = mesh.get_max_on_layer(curr);
    auto delta_y = y_max - y_min;

    // Ось OX будет в центре экрана
    int real_t_start = (int)(len_y * (y_max > 0.0 ? 0.1 : 0.5)); // Положительные значения в верхней части графика
    int real_t_end = (int)(len_y * (y_min < 0.0 ? 0.9 : 0.5)); // Отрицательные в нижней
    int real_yg = real_t_end - real_t_start; // Диапазон позиций поля вывода вдоль OY

    // Масштабирование
    for (size_t i = 0; i < size; i++) {
        auto x = ((abs[i] - x_min) * area_x_diap / delta_x) + area_x_start;
        auto y = ((y_max - layer[i]) * real_yg / delta_y) + real_t_start;
        points[i] = { (float)x, (float)y };
    }

    return points;
}

sdl_grid::sdl_grid(const size_t marks_x, const size_t marks_y, const SDL_FRect area, sdl_text* text) {
    grid_ox_count = marks_x;
    grid_oy_count = marks_y;
    grid_size = 2 * (marks_x + 1) + 2 * (marks_y + 1) + 4;
    coord_grid = new SDL_FPoint[grid_size];
    text_ = text;
    area_ = area;
    size_t total = 0;

    // Расчет середины области графика
    const float mid_y = area.y + area.h * 0.5f;

    const float step_x = area.w / (float)(grid_ox_count);
    const float step_y = area.h / (float)(grid_oy_count);
    const float start_x = area.x; // Отступаем от левого края экрана
    const float start_t = area.y; // Отступаем от верхнего края экрана
    // Отметки на оси абсцисс
    for (size_t i = 0; i < grid_ox_count + 1; i++) {
        float x = (float)i * step_x;

        coord_grid[total++] = { x + start_x, mid_y - 10.0f };
        coord_grid[total++] = { x + start_x, mid_y + 10.0f };
    }
    // Отметки на оси ординат
    for (size_t i = 0; i < grid_oy_count + 1; i++) {
        float t = (float)i * step_y;

        coord_grid[total++] = { start_x - 10.0f, t + start_t };
        coord_grid[total++] = { start_x + 10.0f, t + start_t };
    }
    // Ось абсцисс
    coord_grid[total++] = { area.x, mid_y };
    coord_grid[total++] = { area.x + area.w, mid_y };
    // Ось ординат
    coord_grid[total++] = { start_x, area.y };
    coord_grid[total] = { start_x, area.y + area.h };
}

sdl_grid::~sdl_grid() {
    delete[] coord_grid;
}

void sdl_grid::draw_axes(SDL_Renderer* rend, const mesh_t& mesh, const size_t curr) const {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    for (size_t i = 0; i < grid_size; i += 2) SDL_RenderDrawLinesF(rend, coord_grid + i, 2);

    const float step_x = area_.w / (float)(grid_ox_count);
    const float step_y = area_.h / (float)(grid_oy_count);

    // Получаем данные о сетке
    auto _size_x = mesh.get_size().x;
    auto _layer_x = mesh.get_layer_x();
    const double step_x_axis = (_layer_x[_size_x - 1] - _layer_x[0]) / (double)grid_ox_count;
    const double step_y_axis = (mesh.get_max_on_layer(curr)) / ((double)grid_oy_count / 2.0);

    // Ось абсцисс
    auto pos_y = (area_.h - area_.y) / 2.0 + area_.y + 5;
    for (size_t i = 1; i < grid_ox_count + 1; i++) {
        auto num = text_->cut_number((float)i * step_x_axis, 3);
        text_->render_text(num, (int)((float)i * step_x + area_.x - 9.0f), (int)pos_y);
    }

    // Ось ординат
    auto pos_x = area_.x - 50;
    for (size_t i = 0; i < grid_oy_count + 1; i++) {
        int mult = (int)(grid_oy_count / 2 - i); // Рассчитываем значения ниже OX по значениям выше
        auto num = text_->cut_number(mult * step_y_axis, 3);
        text_->render_text(num, (int)pos_x, (int)((float)i * step_y + area_.y - 9.0f));
    }
}

#endif /* __ENABLE_GRAPH__ */
