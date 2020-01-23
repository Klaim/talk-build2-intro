#include <string>
import print;

int main (int argc, char** argv)
{
    if(argc != 2)
    {
        print_error( "I need a (1) name." );
        return -1;
    }

    print( "Hello, "+ std::string(argv[1]) + "!");
}

