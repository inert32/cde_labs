// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <iostream>
#include <stdexcept>
#include <SDL.h>
#include <SDL_ttf.h>
#include "display.h"

// Очистка экрана
#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

sdl_display::sdl_display() {
    // Запуск SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

    // Создание окна
    window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw std::runtime_error(SDL_GetError());

    // Запуск структуры отрисовки
    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rend == nullptr) throw std::runtime_error(SDL_GetError());

    // Создание координатной сетки
    text = new sdl_text(rend);
    coord_grid = new sdl_grid(10, 10, { area_x_start , area_y_start, area_x_end - area_x_start, area_y_end - area_y_start }, text);

    clear_screen();
    SDL_RenderPresent(rend);
}

sdl_display::~sdl_display() {
    delete text;
    delete coord_grid;
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::show_frame(const mesh_t& mesh, const size_t curr, const double t_step) {
    clear_screen();

    // Рисуем границы графика
    SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
    SDL_Point area[6] = { {area_x_start, area_y_start}, {area_x_start, area_y_end}, {area_x_end, area_y_end}, {area_x_end, area_y_start}, {area_x_start, area_y_start}, {area_x_start, area_y_end}, };
    if (SDL_RenderDrawLines(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    // Рисуем координатную сетку
    coord_grid->draw_axes(rend);

    // Рисуем график
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    const auto points = scale_graph(mesh, curr);
    if (SDL_RenderDrawLinesF(rend, points, (int)mesh.get_size().x) < 0) throw std::runtime_error(SDL_GetError());
    delete[] points;

    // Вывести текущее время
    text->render_text("Time: " + std::to_string(curr * t_step) + " s", area_x_start, (int)(0.91 * len_y));

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

sdl_grid::sdl_grid(const size_t marks_x, const size_t marks_y, const SDL_FRect& area, sdl_text* text_writer) {
    grid_ox_count = marks_x;
    grid_oy_count = marks_y;
    grid_size = 2 * (marks_x + 1) + 2 * (marks_y + 1) + 4;
    coord_grid = new SDL_FPoint[grid_size];
    text = text_writer;
    size_t total = 0;

    // Расчет середины области графика
    const float mid_x = area.x + area.w * 0.5f;
    const float mid_y = area.y + area.h * 0.5f;

    const float step_x = area.w / (float)(grid_ox_count); // Отступаем от начала и середины экрана
    const float step_y = area.h / (float)(grid_oy_count); // Тоже самое для вертикали
    const float start_x = area.x; // Отступаем от левого края экрана
    const float start_t = area.y; // Отступаем от верхнего края экрана
    // Отметки на оси абсцисс
    for (int i = 0; i < grid_ox_count + 1; i++) {
        float x = i * step_x;

        coord_grid[total++] = { x + start_x, mid_y - 10.0f };
        coord_grid[total++] = { x + start_x, mid_y + 10.0f };
    }
    // Отметки на оси ординат
    for (int i = 0; i < grid_oy_count + 1; i++) {
        float t = i * step_y;

        coord_grid[total++] = { mid_x - 10.0f, t + start_t };
        coord_grid[total++] = { mid_x + 10.0f, t + start_t };
    }
    // Оси
    coord_grid[total++] = { area.x, mid_y };
    coord_grid[total++] = { area.x + area.w, mid_y };
    coord_grid[total++] = { mid_x, area.y };
    coord_grid[total] = { mid_x, area.y + area.h };
}

sdl_grid::~sdl_grid() {
    delete[] coord_grid;
}

void sdl_grid::draw_axes(SDL_Renderer* rend) const {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    for (int i = 0; i < grid_size; i += 2) SDL_RenderDrawLinesF(rend, coord_grid + i, 2);
}

sdl_text::sdl_text(SDL_Renderer* renderer) {
    rend = renderer;
    TTF_Init();
    // Загрузка шрифта
    font = TTF_OpenFont("font.ttf", 16);
    if (font == nullptr) throw std::runtime_error(TTF_GetError());
}

sdl_text::~sdl_text() {
    TTF_CloseFont(font);
}

void sdl_text::render_text(const std::string& text, const int x, const int y) {
    auto surf = TTF_RenderText_LCD(font, text.c_str(), {255, 255, 255, 255}, {127, 127, 127, 0});
    if (surf == nullptr) {
        std::cerr << "SDL_ttf: " << TTF_GetError() << std::endl;
        return;
    }
    auto texture = SDL_CreateTextureFromSurface(rend, surf);
	if (texture == nullptr){
		std::cerr << "SDL_ttf: " << TTF_GetError() << std::endl;
        return;
	}

    SDL_Rect dest;
    dest.x = x; dest.y = y;
    TTF_SizeText(font, text.c_str(), &dest.w, &dest.h);
    SDL_RenderCopy(rend, texture, nullptr, &dest);
    SDL_FreeSurface(surf);
}

#endif /* __ENABLE_GRAPH__ */
