#pragma once

#include <chrono>
#include <ctime>
#include <stdio.h>

typedef std::chrono::duration<double, std::ratio<1> > second_;

class TimeClock
{
private:
    std::chrono::time_point<std::chrono::system_clock> startTime;
    std::chrono::time_point<std::chrono::system_clock> endTime;

public:
    TimeClock() {}
    inline void start() { startTime = std::chrono::system_clock::now(); }
    inline void end() { endTime = std::chrono::system_clock::now(); }
    inline double getRoundTripTime() { return(std::chrono::duration_cast<second_>(endTime - startTime).count()); }
};
