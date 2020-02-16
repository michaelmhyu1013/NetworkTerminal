#include <TimeClock.h>


double TimeClock::getRoundTripTime() const// Return in ms
{
    return((std::chrono::duration_cast<second_>(endTime - startTime).count()) * 1000);
}


void TimeClock::resetTime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::system_clock::now();

    startTime = endTime = now;
}
