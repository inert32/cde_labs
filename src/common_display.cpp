// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include "common_display.h"

sdl_text::sdl_text(SDL_Renderer* renderer, const bool force_start) {
    rend = renderer;
    TTF_Init();
    // Загрузка шрифта
    font = TTF_OpenFont("font.ttf", 16);
    if (font == nullptr && !force_start) throw std::runtime_error(TTF_GetError());
}

sdl_text::~sdl_text() {
    TTF_CloseFont(font);
}

void sdl_text::render_text(const std::string& text, const int x, const int y, const int len = 0) {
    if (font == nullptr) return;

    auto surf = TTF_RenderText_LCD(font, text.c_str(), {255, 255, 255, 255}, {127, 127, 127, 255});
    if (surf == nullptr) throw std::runtime_error(TTF_GetError());
    auto texture = SDL_CreateTextureFromSurface(rend, surf);
	if (texture == nullptr) throw std::runtime_error(TTF_GetError());

    SDL_Rect dest;
    TTF_SizeText(font, text.c_str(), &dest.w, &dest.h);
    dest.y = y;
    dest.x = x;
    if (len > 0) dest.x += (len - dest.w) / 2;

    SDL_RenderCopy(rend, texture, nullptr, &dest);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(texture);
}

std::string sdl_text::cut_number(const double num, const size_t size) const {
    std::string ret = std::to_string(num);
    auto point_pos = ret.find('.');
    if (point_pos == ret.npos || point_pos + size > ret.size()) return ret;
    else return ret.substr(0, point_pos + size);
}

#endif /* __ENABLE_GRAPH__ */