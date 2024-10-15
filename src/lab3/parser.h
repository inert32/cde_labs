#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>
#include <string>
#include "model.h"

// Массив параметров задания
// std::map не подходит, так как в файле может быть несколько subarea
typedef std::vector<std::pair<std::string, std::string>> parser_data;

// Парсер файла задания
parser_data parse_task_file(const std::string& path);

// Создание областей работы
std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area);

// Создание источника частиц
emit_point* spawn_emitter(const parser_data& src);

// Получение числа частиц для обработки
size_t get_particles_count(const parser_data& src);

#endif /* __PARSER_H__ */