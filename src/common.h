#ifndef __COMMON_H__
#define __COMMON_H__

#include <map>
#include <string>

typedef std::map<std::string, std::string, std::less<>> cli_map;
cli_map parse_cli(int argc, char** argv);

#endif /* __COMMON_H__ */
