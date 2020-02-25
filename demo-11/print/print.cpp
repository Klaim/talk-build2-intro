#include "print.hpp"

#include <iostream>
#include <mutex>
// #include <fmt/format.hpp>

static std::mutex print_mutex;

void print(std::string input)
{
    std::scoped_lock lock{print_mutex};
    std::cout << "> " << input << std::endl;
}

void print_error(std::string input)
{
    std::scoped_lock lock{print_mutex};
    std::cerr << "> " << input << std::endl;
}
