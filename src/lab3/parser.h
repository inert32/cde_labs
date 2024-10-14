#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>
#include <string>
#include "model.h"

typedef std::vector<std::pair<std::string, std::string>> parser_data;

// Парсер файла задания
parser_data parse_task_file(const std::string& path);

// Создание областей работы
std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area);

#endif /* __PARSER_H__ */