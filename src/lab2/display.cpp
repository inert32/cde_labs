// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "display.h"

sdl_display::sdl_display() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::string err = SDL_GetError();
        throw std::runtime_error(err);
    }
    window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::string err = SDL_GetError();
        throw std::runtime_error(err);
    }
    surf = SDL_GetWindowSurface(window);
    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x7F, 0x7F, 0x7F));
    SDL_UpdateWindowSurface(window);
}

sdl_display::~sdl_display() {
    SDL_FreeSurface(surf);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::show_frame() {

}

sdl_events handle_kbd() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return sdl_events::quit;
        if (e.type == SDL_KEYDOWN) return sdl_events::quit;
    }
    return sdl_events::none;
}
