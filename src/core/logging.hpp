#pragma once

#include <iostream>

// colored printing for very basic logging
namespace LOG {
    static const char* reset = "\33[0m";

    static const char* black = "\33[0;30m";
    static const char* BLACK = "\33[1;30m";

    static const char* red = "\33[0;31m";
    static const char* RED = "\33[1;31m";

    static const char* green = "\33[0;32m";
    static const char* GREEN = "\33[1;32m";

    static const char* yellow = "\33[0;33m";
    static const char* YELLOW = "\33[1;33m";

    static const char* blue = "\33[0;34m";
    static const char* BLUE = "\33[1;34m";

    static const char* magenta = "\33[0;35m";
    static const char* MAGENTA = "\33[1;35m";

    static const char* cyan = "\33[0;36m";
    static const char* CYAN = "\33[1;36m";

    static const char* white = "\33[0;37m";
    static const char* WHITE = "\33[1;37m";

    [[maybe_unused]] static std::ostream& __log(const char* color) {
        return (std::cout << color);
    }

    #define DEBUG LOG::__log(LOG::white)
    #define INFO  LOG::__log(LOG::cyan)
    #define WARN  LOG::__log(LOG::yellow)
    #define ERROR LOG::__log(LOG::red)
    #define ENDLOG LOG::_log(LOG::reset) << std::endl
}