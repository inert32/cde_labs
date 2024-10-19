#ifndef __PARSER_TYPES_H__
#define __PARSER_TYPES_H__

#include <vector>
#include <string>

// Массив параметров задания
// std::map не подходит, так как в файле может быть несколько subarea
typedef std::vector<std::string> parser_opts;
typedef std::pair<std::string, parser_opts> parser_line;
typedef std::vector<parser_line> parser_data;

#endif /* __PARSER_TYPES_H__ */
