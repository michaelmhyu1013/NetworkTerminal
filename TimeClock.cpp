#include <TimeClock.h>


double TimeClock::getRoundTripTime() const// Return in ms
{
    return((std::chrono::duration_cast<second_>(endTime - startTime).count()) * 1000);
}
