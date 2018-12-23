#include <iostream>
#include <sstream>

#include "device.h"

int main( int argc, char* argv[] )
{
    try{
        Device device;
        device.run();
    } catch( std::exception& ex ){
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
