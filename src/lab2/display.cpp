// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>

#include <stdexcept>
#include <string>
#include <SDL2/SDL_render.h>
#include "display.h"

sdl_display::sdl_display() {
    // Запуск SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

    // Создание окна
    window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw std::runtime_error(SDL_GetError());

    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    if (rend == nullptr) throw std::runtime_error(SDL_GetError());
    SDL_SetRenderDrawColor(rend, 127, 127, 127, 255);
    SDL_RenderClear(rend);
    SDL_RenderPresent(rend);
}

sdl_display::~sdl_display() {
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::show_frame(const size_t curr) {
    // Sample!
    SDL_SetRenderDrawColor(rend, 127, 127, 127, 255);
    SDL_RenderClear(rend);
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    SDL_FPoint points[10];
    for (int i = 0; i < 10; i++) {
        float x = (float)(10 + curr + i);
        float y = 1.1f * i;
        points[i] = {x, y};
        std::cout << "{" << x << " " << y << "} ";
    }
    std::cout << std::endl;
    auto ret = SDL_RenderDrawPointsF(rend, points, 9);
    std::cout << "show_frame: ret: " << ret << ", curr: " << curr << std::endl;
    if (ret < 0) throw std::runtime_error(SDL_GetError());
    SDL_RenderPresent(rend);
}

sdl_events handle_kbd() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return sdl_events::quit;
        if (e.type == SDL_KEYDOWN) return sdl_events::next;
    }
    return sdl_events::none;
}
