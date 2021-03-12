#include <unistd.h>
#include <cstddef>
#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Constructor for system
System::System()
{
    std::vector<int> pids = LinuxParser::Pids();
    for (size_t index = 0; index < pids.size(); index++)
        processes_.push_back(Process(pids[index]));
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() 
{ 
    // Sort processes based on their cpu utilization
    std::sort(processes_.begin(), processes_.end(), [](const auto& a, const auto& b) 
    {
        return a < b;
    });
    return processes_; 
}

// Return the request system's kernel identifier (string) from LinuxParser
std::string System::Kernel() const { return LinuxParser::Kernel(); }

// Return the system's memory utilization from LinuxParser
float System::MemoryUtilization() const { return LinuxParser::MemoryUtilization(); }

// Return request of the operating system name from LinuxParser
std::string System::OperatingSystem() const { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() const { return LinuxParser::UpTime(); }