// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "../build.h"

int main() {
    std::cout << "Lab1 " << build_version << " " << build_git << std::endl;
    return 0;
}