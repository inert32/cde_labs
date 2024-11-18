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
