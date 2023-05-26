#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>
#include <iostream>

class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::string name;

public:
    Timer(const std::string& name_);
    ~Timer();
};

#endif // TIMER_H