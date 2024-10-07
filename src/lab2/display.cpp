// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include <SDL.h>
#include "display.h"

#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

sdl_display::sdl_display() {
    // Запуск SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

    // Создание окна
    window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw std::runtime_error(SDL_GetError());

    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    if (rend == nullptr) throw std::runtime_error(SDL_GetError());
    clear_screen();
    SDL_RenderPresent(rend);
}

sdl_display::~sdl_display() {
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::show_frame(const mesh_t& mesh, const size_t curr) {
    clear_screen();
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);

    // Создаем точки
    auto size = mesh.get_size().x;
    SDL_FPoint* points = new SDL_FPoint[size];

    auto abs = mesh.get_layer_x();
    auto layer = mesh.get_layer(curr);

    for (size_t i = 0; i < size; i++) {
        float x = (float)abs[i] * 50.0f;
        float y = (float)layer[i] * 50.0f;
        points[i] = {x, y};
    }
    if (SDL_RenderDrawLinesF(rend, points, size) < 0) throw std::runtime_error(SDL_GetError());
    SDL_RenderPresent(rend);
}

sdl_events handle_kbd() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return sdl_events::quit;
        if (e.type == SDL_KEYDOWN) {
            switch (SDL_GetKeyFromScancode(e.key.keysym.scancode)) {
                case 27:         return sdl_events::quit;     // Escape
                case 1073741903: return sdl_events::next;     // Стрелка вправо
                case 1073741904: return sdl_events::previous; // Стрелка влево
                default: break;
            }
        }
    }
    return sdl_events::none;
}

#endif /* __ENABLE_GRAPH__ */
