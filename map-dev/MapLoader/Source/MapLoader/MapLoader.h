#pragma once
#include <exception>
#include "../Matrix/Matrix.h"


class MapLoader {
    
    private:

    public:
    MapLoader() = default;
};

class IndexOutOfRangeException : std::exception {
    public: 
    const char * what();
};
