#ifndef __MESH_H__
#define __MESH_H__

#include <string>

// Сетка
class mesh_t {
public:
    // Создать равномерную сетку размером x_size на t_size
    mesh_t(const size_t x_size, const size_t t_size) {
        _x_size = x_size; _t_size = t_size;
        mesh = new double*[t_size];
        for (size_t t = 0; t < t_size; t++) {
            mesh[t] = new double[x_size];
            for (size_t x = 0; x < x_size; x++) mesh[t][x] = 0.0;
        }

        layer_x = new double[x_size];
        for (size_t x = 0; x < x_size; x++) layer_x[x] = 0.0;
    }
    ~mesh_t() {
        for (size_t t = 0; t < _t_size; t++) delete[] mesh[t];
        delete[] mesh;
        delete[] layer_x;
    }

    // Получить рассчитанную сетку
    const double** get_mesh() const {
        return (const double**)mesh;
    }

    // Получить рассчитанный слой
    const double* get_layer(const size_t t) const {
        if (t >= _t_size) {
            std::string msg = "mesh: Error: Required layer " + std::to_string(t) +
            " is out of range";
            throw std::runtime_error(msg);
        }
        return (const double*)mesh[t];
    }

    // Получить значения оси абсцисс
    const double* get_layer_x() const {
        return (const double*)layer_x;
    }

    // Задать значения оси абсцисс
    void set_layer_x(const double step) {
        for (size_t i = 0; i < _x_size; i++) layer_x[i] = i * step;
    }

    // Задать первый слой сетки
    void set_layer_t0(double (*func)(double)) {
        for (size_t i = 0; i < _x_size; i++) mesh[0][i] = func(layer_x[i]);
    }

    // Вычислить слой t по предыдущему
    void calc_layer(const size_t t, const double courant) {
        if (t == 0) return;
        if (t >= _t_size) {
            std::string msg = "mesh: Error: Required layer " + std::to_string(t) +
            " is out of range";
            throw std::runtime_error(msg);
        }

        mesh[t][0] = mesh[t - 1][_x_size - 1];
        for (size_t x = 1; x < _x_size; x++)
            mesh[t][x] = mesh[t - 1][x] - courant * (mesh[t - 1][x] - mesh[t - 1][x - 1]);
    }

    // Получить максимальное значение функции на слое t
    double get_max_on_layer(const size_t t) const {
        if (t >= _t_size) {
            std::string msg = "mesh: Error: Required layer " + std::to_string(t) +
                " is out of range";
            throw std::runtime_error(msg);
        }

        const auto layer = mesh[t];
        double ret = layer[0];

        for (size_t x = 1; x < _x_size; x++)
            if (layer[x] > ret) ret = layer[x];

        return ret;
    }

    // Получить минимальное значение функции на слое t
    double get_min_on_layer(const size_t t) const {
        if (t >= _t_size) {
            std::string msg = "mesh: Error: Required layer " + std::to_string(t) +
                " is out of range";
            throw std::runtime_error(msg);
        }

        const auto layer = mesh[t];
        double ret = layer[0];

        for (size_t x = 1; x < _x_size; x++)
            if (layer[x] < ret) ret = layer[x];

        return ret;
    }

    struct mesh_size_t {const size_t x; const size_t t; };

    // Получить размер сетки
    const mesh_size_t get_size() const {
        return { _x_size, _t_size };
    }

private:
    double** mesh; // Сетка
    double* layer_x; // Слой с значениями X
    size_t _x_size, _t_size;
};

#endif /* __MESH_H__ */
