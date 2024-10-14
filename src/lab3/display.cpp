// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <iostream>
#include <stdexcept>
#include <SDL.h>
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

    clear_screen();
    SDL_RenderPresent(rend);
}

sdl_display::~sdl_display() {
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::show_frame() {
    clear_screen();

    // Рисуем границы графика
    SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
    SDL_Point area[6] = { {area_x_start, area_y_start}, {area_x_start, area_y_end}, {area_x_end, area_y_end}, {area_x_end, area_y_start}, {area_x_start, area_y_start}, {area_x_start, area_y_end}, };
    if (SDL_RenderDrawLines(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    // Передаем на экран
    SDL_RenderPresent(rend);
}

#endif /* __ENABLE_GRAPH__ */
