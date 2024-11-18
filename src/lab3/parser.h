/*
Laboratory work on numerical methods for solving convection-diffusion equations
Copyright (C) 2024 Mikhaylov Alexey (inert32@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "parser_types.h"
#include "model.h"

const parser_line& find_config_line(const parser_data& conf, const std::string& line);

// Парсер файла задания
parser_data parse_task_file(const std::string& path);

// Создание областей работы
std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area, const mat_t& materials);

// Создание источника частиц
emit_point* spawn_emitter(const parser_line& src, const parser_line& energy);

// Получение числа частиц для обработки
size_t get_particles_count(const parser_line& src);

// Загрузка данных о материале из файла
mat_t load_materials(const parser_data& conf);

#endif /* __PARSER_H__ */