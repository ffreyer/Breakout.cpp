#pragma once

#include <chrono>
#include <iostream>

struct ScopedTimer {
    std::string _name;
    std::chrono::time_point<std::chrono::steady_clock>  _time;

    ScopedTimer(std::string name) {
        _name = name;
        _time = std::chrono::steady_clock::now();
    }

    ~ScopedTimer() {
        auto t2 = std::chrono::steady_clock::now();
        auto elapsed = 
            std::chrono::time_point_cast<std::chrono::microseconds>(_time).time_since_epoch() - 
            std::chrono::time_point_cast<std::chrono::microseconds>(t2).time_since_epoch();

        std::cout << "Timing: " << elapsed.count() << "µs @ " << _name << std::endl;
    }
};