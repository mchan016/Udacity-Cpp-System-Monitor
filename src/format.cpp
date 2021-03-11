#include <string>

#include "format.h"

using std::string;

/*
* ElapsedTime
*
* Input: Long int measuring seconds
*
* Output: Time string formatted as "HH:MM:SS"
*/
string Format::ElapsedTime(long seconds) 
{
    long secondsPerHour = 60 * 60;
    long secondsPerMin = 60;

    string hours = std::to_string(seconds / secondsPerHour);
    seconds %= secondsPerHour;
    string minutes = std::to_string(seconds / secondsPerMin);
    seconds %= secondsPerMin;

    return hours + ':' + minutes + ':' + std::to_string(seconds);
}