#ifndef __DISPLAY_H__
#define __DISPLAY_H__

enum class sdl_events {
    none,
    quit,
    previous,
    next
};

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include <SDL2/SDL_render.h>
#include "mesh.h"

// Отображение графика на экране
class sdl_display {
public:
    sdl_display();
    ~sdl_display();

    // Выбор слоя сетки (curr) для отображения
    void show_frame(const mesh_t& mesh, const size_t curr);

private:
    const size_t len_x = 1024, len_y = 768;
    SDL_Window* window = nullptr;
    SDL_Renderer* rend = nullptr;
};

sdl_events handle_kbd();

#else
#include <iostream>
#include "mesh.h"

class sdl_display {
public:
    sdl_display() { std::cout << "SDL: display disabled. " << std::endl; }
    ~sdl_display() {}

    void show_frame([[maybe_unused]] const mesh_t& mesh, [[maybe_unused]] const size_t curr) {}
};

sdl_events handle_kbd() { return sdl_events::quit; }

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
