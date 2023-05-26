#include "Timer.h"

Timer::Timer(const std::string& name_) : name(name_), start(std::chrono::high_resolution_clock::now()) {}
    
Timer::~Timer() {
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration<double, std::milli>(end - start);
    std::cout << name << " finished after: " << duration.count() << " ms" << std::endl;
}