#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdexcept>
#include <SDL2/SDL_render.h>

enum class sdl_events {
    none,
    quit,
    previous,
    next
};

// Отображение графика на экране
class sdl_display {
public:
    sdl_display();
    ~sdl_display();

    // Выбор слоя сетки (curr) для отображения
    void show_frame(const size_t curr);

private:
    const size_t len_x = 1024, len_y = 768;
    SDL_Window* window = nullptr;
    SDL_Renderer* rend = nullptr;
};

sdl_events handle_kbd();

#endif /* __DISPLAY_H__ */
