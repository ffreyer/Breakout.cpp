#pragma once

#include <iostream>

// colored printing for very basic logging
namespace LOG {
    static const char* reset = "\e[0m";

    static const char* black = "\e[0;30m";
    static const char* BLACK = "\e[1;30m";

    static const char* red = "\e[0;31m";
    static const char* RED = "\e[1;31m";

    static const char* green = "\e[0;32m";
    static const char* GREEN = "\e[1;32m";

    static const char* yellow = "\e[0;33m";
    static const char* YELLOW = "\e[1;33m";

    static const char* blue = "\e[0;34m";
    static const char* BLUE = "\e[1;34m";

    static const char* magenta = "\e[0;35m";
    static const char* MAGENTA = "\e[1;35m";

    static const char* cyan = "\e[0;36m";
    static const char* CYAN = "\e[1;36m";

    static const char* white = "\e[0;37m";
    static const char* WHITE = "\e[1;37m";

    static std::ostream& __log(const char* color) {
        return (std::cout << color);
    }

    #define DEBUG LOG::__log(LOG::white)
    #define INFO  LOG::__log(LOG::cyan)
    #define WARN  LOG::__log(LOG::yellow)
    #define ERROR LOG::__log(LOG::red)
    #define ENDLOG LOG::_log(LOG::reset) << std::endl
}

