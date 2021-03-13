#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
std::vector<Process>& System::Processes() 
{ 
    processes_.clear();

    std::vector<int> pids = LinuxParser::Pids();
    processes_.reserve(pids.size());
    for (const auto& pid : pids)
        processes_.emplace_back(pid);
    
    // Sort processes based on their cpu utilization
    std::sort(processes_.begin(), processes_.end());
    
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