#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "processor.h"

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
    // prepare container for the ten CPU fields
    const size_t cpuElements{10};
    std::vector<size_t> fields(cpuElements, 0);

    std::string path = "/proc/stat";
    std::ifstream stream(path);
    std::string line, category;
    if (stream.is_open())
    {
        while (getline(stream, line))
        {
            std::istringstream fileStream(line);
            fileStream >> category;
            if (category == "cpu")
            {
                for (size_t index = 0; index < cpuElements; index++)
                    fileStream >> fields[index];

                // idle + iowait
                size_t idle = fields[3] + fields[4];
                // user + nice + system + irq + softirq + steal 
                size_t nonIdle = fields[0] + fields[1] + fields[2] + 
                                 fields[5] + fields[6] + fields[7];
                
                _cpuPercentage = static_cast<float>(nonIdle) / (idle + nonIdle);
                break;
            }
        }
    }

    return _cpuPercentage; 
}