#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>
#include <string>
#include "parser_types.h"
#include "model.h"

const parser_line& find_config_line(const parser_data& conf, const std::string& line);

// Парсер файла задания
parser_data parse_task_file(const std::string& path);

// Создание областей работы
std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area);

// Создание источника частиц
emit_point* spawn_emitter(const parser_line& src);

// Получение числа частиц для обработки
size_t get_particles_count(const parser_line& src);

#endif /* __PARSER_H__ */