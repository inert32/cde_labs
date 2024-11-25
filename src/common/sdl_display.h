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
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

// Очистка экрана
#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

class sdl_text {
public:
    // Конструктор sdl_text. Класс требует файл шрифта 'font.ttf' в том же
    // каталоге, что и исполняемый файл. Флаг force_start позволит
    // запустить sdl_text без шрифта, но render_text не будет выводить текст.
    sdl_text(SDL_Renderer* renderer, const bool force_start = false) {
        rend = renderer;
        TTF_Init();
        // Загрузка шрифта
        font = TTF_OpenFont("font.ttf", 16);
        if (font == nullptr && !force_start) throw std::runtime_error(TTF_GetError());
    }
    ~sdl_text() { TTF_CloseFont(font); }

    // Вывод текста по координатам x, y.
    // Переменная len позволит выровнять текст по центру
    void render_text(const std::string& text, const int x, const int y, const int len = 0) {
        if (font == nullptr) return;

        auto surf = TTF_RenderText_LCD(font, text.c_str(), {255, 255, 255, 255}, {127, 127, 127, 255});
        if (surf == nullptr) throw std::runtime_error(TTF_GetError());
        auto texture = SDL_CreateTextureFromSurface(rend, surf);
	    if (texture == nullptr) throw std::runtime_error(TTF_GetError());

        SDL_Rect dest;
        TTF_SizeText(font, text.c_str(), &dest.w, &dest.h);
        dest.y = y; dest.x = x;
        if (len > 0) dest.x += (len - dest.w) / 2;

        SDL_RenderCopy(rend, texture, nullptr, &dest);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(texture);
    }

    // Для чисел с плавающей запятой отрезать (не округлить)
    // лишнее число (size) знаков после запятой
    inline std::string cut_number(const double num, const size_t size) const {
        std::string ret = std::to_string(num);
        auto point_pos = ret.find('.');
        if (point_pos == ret.npos || point_pos + size > ret.size()) return ret;
        else return ret.substr(0, point_pos + size);
    }
private:
    // Шрифт
    TTF_Font* font = nullptr;
    SDL_Renderer* rend = nullptr;
};

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