#ifndef __MODEL_H__
#define __MODEL_H__

// Главная область моделирования
struct main_area_t {
    float width = 0.0f;
    float height = 0.0f;
};

// Подобласть моделирования внутри 
struct subarea_t {
    float x_start = 0.0f;
    float width = 0.0f;
};

struct vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

class particle {
public:
    particle(const float pos_x, const float pos_y, const float vel_x, const float vel_y);

    vec2 get_position() const;
    vec2 get_velocity() const;

    void move_particle(const float len);
    void set_velocity(const float vel_x, const float vel_y);
private:
    // Координаты частицы
    vec2 pos;
    // Вектор скорости
    vec2 velocity;
};

// Точечный источник частиц
class emit_point {
public:
    emit_point(const float pos_x, const float pos_y);
    emit_point() = default;

    particle spawn_particle();
    vec2 get_position() const;

private:
    vec2 pos;
    float last_angle = 45.0f;
};

#endif /* __MODEL_H__ */