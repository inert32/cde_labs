#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>
#include <string>
#include <map>
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

mat_t load_materials(const parser_data& conf);

#endif /* __PARSER_H__ */