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
    sdl_text(SDL_Renderer* renderer, const bool force_start = false);
    ~sdl_text();

    // Вывод текста по координатам x, y.
    // Переменная len позволит выровнять текст по центру
    void render_text(const std::string& text, const int x, const int y, const int len);

    // Для чисел с плавающей запятой отрезать (не округлить)
    // лишнее число (size) знаков после запятой
    std::string cut_number(const double num, const size_t size) const;
private:
    // Шрифт
    TTF_Font* font = nullptr;
    SDL_Renderer* rend = nullptr;
};

#endif /* __ENABLE_GRAPH__ */

#endif /* __COMMON_DISPLAY_H__ */