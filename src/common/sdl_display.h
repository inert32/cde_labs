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
#ifndef __COMMON_SDL_DISPLAY_H__
#define __COMMON_SDL_DISPLAY_H__

#ifdef __ENABLE_GRAPH__

#include <iostream>
#include <stdexcept>
#include <SDL.h>
#include "sdl_text.h"

// Очистка экрана
#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

class sdl_display_base {
public:
    sdl_display_base(const std::string_view title, const int length = 1024, const int width = 768) {
        len_x = length;
        len_y = width;
        // Запуск SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

        // Создание окна
        window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
        if (window == nullptr) throw std::runtime_error(SDL_GetError());

        // Запуск структуры отрисовки
        rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (rend == nullptr) throw std::runtime_error(SDL_GetError());
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

        // Расчет областей окна
        area_x_start = 0.1f * (float)len_x;
        area_x_end = 0.9f * (float)len_x;
        area_x_diap = area_x_end - area_x_start;

        area_y_start = 0.1f * (float)len_y;
        area_y_end = 0.9f * (float)len_y;
        area_y_diap = area_y_end - area_y_start;

        // Отрисовка текста
        try {
            text = new sdl_text(rend);
        }
        catch (const std::exception &e) {
            std::cerr << "SDL: Warn: No font.ttf detected, disable text functions." << std::endl;
            text = new sdl_text(rend, true);
        }

        clear_screen();
        SDL_RenderPresent(rend);
    }

    virtual ~sdl_display_base() {
        delete text;
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
protected:
    int len_x, len_y; // Размер окна
    SDL_Window* window = nullptr;
    SDL_Renderer* rend = nullptr;

    // Область рисования
    float area_x_start;
    float area_x_end;
    float area_x_diap;

    float area_y_start;
    float area_y_end;
    float area_y_diap;

    sdl_text* text;
};
#else
class sdl_display_base {
public:
    sdl_display_base([[maybe_unused]] const std::string_view title = "", 
    [[maybe_unused]] const int length = 1024, [[maybe_unused]] const int width = 768) { }
    virtual ~sdl_display_base() = default;
};

#endif /* __ENABLE_GRAPH__ */

#endif /* __COMMON_SDL_DISPLAY_H__ */