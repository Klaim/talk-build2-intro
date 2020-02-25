#include <iostream>

int main (int argc, char** argv)
{
    if(argc != 2)
    {
        std::cerr << "I need a (1) name." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Hello, " << argv[1] << "!" << std::endl;
}

