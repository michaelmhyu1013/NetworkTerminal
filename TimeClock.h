#pragma once

#include <chrono>
#include <ctime>
#include <stdio.h>
#include <string>
typedef std::chrono::duration<double, std::ratio<1> > second_;

class TimeClock
{
private:
    TimeClock() {}
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

public:
    static TimeClock &getInstance()
    {
        static TimeClock theInstance;

        return(theInstance);
    }
    inline void start() { startTime = std::chrono::system_clock::now(); }
    inline void end() { endTime = std::chrono::system_clock::now(); }
    double getRoundTripTime() const;
    void resetTime();
    std::string &printRoundTripTime() const;
};
