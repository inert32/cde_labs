#ifndef __COMMON_H__
#define __COMMON_H__

#include <map>
#include <string>

// Работа с аргументами командной строки
typedef std::map<std::string, std::string, std::less<>> cli_map;
cli_map parse_cli(int argc, char** argv);

double double_from_cli_map(const cli_map& cli, const std::string& key, const double default_value = 0.0);
size_t sizet_from_cli_map(const cli_map& cli, const std::string& key, const size_t default_value = 0);
std::string str_from_cli_map(const cli_map& cli, const std::string& key, const std::string& default_value = "");

#endif /* __COMMON_H__ */
