#ifndef __LAB2__MESH_H__
#define __LAB2__MESH_H__

#include <stddef.h>

// Сетка
class mesh_t {
public:
    mesh_t(const size_t x_size, const size_t t_size) {
        _x_size = x_size; _t_size = t_size;
        mesh = new double*[t_size];
        for (size_t t = 0; t < t_size; t++) {
            mesh[t] = new double[x_size];
            for (size_t x = 0; x < x_size; x++) mesh[t][x] = 0.0;
        }
    }
    ~mesh_t() {
        for (size_t t = 0; t < _t_size; t++) delete[] mesh[t];
        delete[] mesh;
    }

    double** get_mesh() {
        return mesh;
    }

    struct mesh_size_t {const size_t x; const size_t t; };

    mesh_size_t get_size() const {
        return { _x_size, _t_size };
    }

private:
    double** mesh;
    size_t _x_size, _t_size;
};

#endif /* __LAB2__MESH_H__ */