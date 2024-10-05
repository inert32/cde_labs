#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdexcept>
#include <string>
#include <SDL.h>
#include "mesh.h"

class sdl_display {
public:
    sdl_display();
    ~sdl_display();

    void start_window();

private:
    const size_t len_x = 1024, len_y = 768;
    SDL_Window* window = nullptr;
    SDL_Surface* surf = nullptr;
};

#endif /* __DISPLAY_H__ */
