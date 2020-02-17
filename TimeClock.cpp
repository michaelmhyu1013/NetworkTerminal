#include <TimeClock.h>
/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		WindowsThreadService.cpp -	A static member that performs system clock recording.
 * --
 * -- PROGRAM:			NetworkTerminal
 * --
 * -- DATE:             Feb 05, 2020
 * --
 * -- FUNCTIONS:
 * --                   double getRoundTripTime()
 * --                   void start()
 * --                   void end()
 * --                   void resetTime()
 * --
 * -- REVISIONS:
 * --		NA
 * --
 * -- DESIGNER:         Michael Yu
 * --
 * -- PROGRAMMER:		Michael Yu
 * --
 * -- NOTES:
 * -- This class allows the user to record a starting and ending time of the system clock. It exposes a getRoundTripTime
 * -- that calculates the time difference in milliseconds between the start and end time of the stored values.
 * ----------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: getRoundTripTime
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: double getRoundTripTime(void)
 * --
 * -- RETURNS: double representing the time difference between the end and start time of the static instance
 * --
 * -- NOTES:
 * -- The start and end times must be properly reset and set in order to get an accurate time difference between the ending
 * -- and start time.
 * ----------------------------------------------------------------------------------------------------------------------*/
double TimeClock::getRoundTripTime() const// Return in ms
{
    return((std::chrono::duration_cast<second_>(endTime - startTime).count()) * 1000);
}

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: getRoundTripTime
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void resetTime(void)
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Simply sets the start and end time of the static instance to the same value.
 * ----------------------------------------------------------------------------------------------------------------------*/
void TimeClock::resetTime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::system_clock::now();

    startTime = endTime = now;
}
