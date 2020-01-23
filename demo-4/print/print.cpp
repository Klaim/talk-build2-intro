#include "print.hpp"

#include <iostream>

void print(std::string input)
{
    std::cout << input << std::endl;
}

void print_error(std::string input)
{
    std::cerr << input << std::endl;
}
