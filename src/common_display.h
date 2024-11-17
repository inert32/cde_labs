#ifndef __COMMON_DISPLAY_H__
#define __COMMON_DISPLAY_H__

#ifdef __ENABLE_GRAPH__

#include <string>
#include <SDL_ttf.h>
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

#endif /* __ENABLE_GRAPH__ */

#endif /* __COMMON_DISPLAY_H__ */