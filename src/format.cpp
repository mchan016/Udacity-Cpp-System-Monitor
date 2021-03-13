#include <iomanip>
#include <sstream>
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
    long h = seconds / 3600;
    long m = (seconds / 3600) / 60;
    long s = (seconds % 3600 % 60);

    std::ostringstream elapsedTime;
    elapsedTime << std::setw(2) << std::setfill('0') << h
            << ':' << std::setw(2) << std::setfill('0') << m 
            << ':' << std::setw(2) << std::setfill('0') << s;

    return elapsedTime.str();
}