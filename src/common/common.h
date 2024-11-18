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
#ifndef __COMMON_H__
#define __COMMON_H__

#include <map>
#include <string>

// Работа с аргументами командной строки
typedef std::map<std::string, std::string, std::less<>> cli_map;
cli_map parse_cli(int argc, char** argv);

// Перевод значения параметра в типы
double double_from_cli_map(const cli_map& cli, const std::string& key, const double default_value = 0.0);
size_t sizet_from_cli_map(const cli_map& cli, const std::string& key, const size_t default_value = 0);
std::string str_from_cli_map(const cli_map& cli, const std::string& key, const std::string& default_value = "");
bool bool_from_cli_map(const cli_map& cli, const std::string& key, const bool default_value);

#endif /* __COMMON_H__ */
