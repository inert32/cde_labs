// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "../build.h"
#include "../common.h"

int main(int argc, char** argv) {
    std::cout << "Lab2 " << build_version << " " << build_git << std::endl;
    return 0;
}