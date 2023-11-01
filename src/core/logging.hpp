#pragma once

#include <iostream>

// colored printing for very basic logging
namespace LOG {
    const char* black = "\e[0;30m";
    const char* BLACK = "\e[1;30m";

    const char* red = "\e[0;31m";
    const char* RED = "\e[1;31m";

    const char* green = "\e[0;32m";
    const char* GREEN = "\e[1;32m";

    const char* yellow = "\e[0;33m";
    const char* YELLOW = "\e[1;33m";

    const char* blue = "\e[0;34m";
    const char* BLUE = "\e[1;34m";

    const char* magenta = "\e[0;35m";
    const char* MAGENTA = "\e[1;35m";

    const char* cyan = "\e[0;36m";
    const char* CYAN = "\e[1;36m";

    const char* white = "\e[0;37m";
    const char* WHITE = "\e[1;37m";

    std::ostream& __log(const char* color) {
        return (std::cout << color);
    }

    #define DEBUG LOG::__log(LOG::white)
    #define INFO  LOG::__log(LOG::cyan)
    #define WARN  LOG::__log(LOG::yellow)
    #define ERROR LOG::__log(LOG::red)
}

