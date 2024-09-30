// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <cmath>
#include "../build.h"
#include "../common.h"

double func1(const double x) {
    if (x > 2.0 && x < 5.0) return 1.0;
    else return 0.0;
}

int main(int argc, char** argv) {
    std::cout << "Lab2 " << build_version << " " << build_git << std::endl;
    const auto cli = parse_cli(argc, argv);
    
    const double sampling_rate = 0.1;

    std::cout << "x\ty" << std::endl;
    for (double x = 0.0; x < 10.0; x+=sampling_rate)
        std::cout << x << '\t' << func1(x) << std::endl;

    return 0;
}
