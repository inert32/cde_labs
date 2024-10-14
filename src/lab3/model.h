#ifndef __MODEL_H__
#define __MODEL_H__

// Область моделирования
struct area {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
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

// 
class emit_base {
public:
    virtual ~emit_base() = default;

    virtual particle spawn_particle() = 0;
};

// Точечный источник частиц
class emit_point : emit_base {
public:
    emit_point(const float pos_x, const float pos_y);
    emit_point() = default;

    particle spawn_particle();

private:
    float x, y;
    float last_angle = 45.0f;
};

#endif /* __MODEL_H__ */