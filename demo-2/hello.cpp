#include <cstdlib>
#include "print.hpp"

int main (int argc, char** argv)
{
    if(argc != 2)
    {
        print_error( "I need a (1) name." );
        return EXIT_FAILURE;
    }

    print( "Hello, "+ std::string(argv[1]) + "!");
}

