// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "../build.h"
#include "../common/common.h"

int main(int argc, char** argv) {
    std::cout << "Lab4 " << build_version << " " << build_git << std::endl;
    auto cli = parse_cli(argc, argv);
    const bool run_sdl = bool_from_cli_map(cli, "graph", true);

    return 0;
}