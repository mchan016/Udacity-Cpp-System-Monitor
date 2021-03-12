#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "processor.h"
#include "linux_parser.h"

/* 
* Utilization
* 
* Return the aggregate CPU utilization
* 
* CPU is represented by 10 different fields:
*  0   |  1   |   2    |  3   |   4    |  5  |    6    |   7   |   8   |     9
* user | nice | system | idle | iowait | irq | softirq | steal | guest | guest_nice
*/
float Processor::Utilization()
{
    return LinuxParser::CpuUtilization().at(0); 
}