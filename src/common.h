#ifndef __COMMON_H__
#define __COMMON_H__

#include <map>
#include <string>

std::map<std::string, std::string, std::less<>> parse_cli(int argc, char** argv);

#endif /* __COMMON_H__ */
