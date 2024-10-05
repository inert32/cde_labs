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
}

sdl_display::~sdl_display() {
    SDL_FreeSurface(surf);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::start_window() {
    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x7F, 0x7F, 0x7F));
    SDL_UpdateWindowSurface( window );
    SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
}
