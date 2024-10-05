#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdexcept>
#include <string>
#include <SDL.h>
#include "mesh.h"

enum class sdl_events {
    none,
    quit,
    previous,
    next
};

class sdl_display {
public:
    sdl_display();
    ~sdl_display();

    void show_frame();

private:
    const size_t len_x = 1024, len_y = 768;
    SDL_Window* window = nullptr;
    SDL_Surface* surf = nullptr;
};

sdl_events handle_kbd();

#endif /* __DISPLAY_H__ */
