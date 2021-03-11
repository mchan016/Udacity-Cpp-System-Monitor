#include <unistd.h>
#include <cstddef>
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

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// Return the request system's kernel identifier (string) from LinuxParser
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization from LinuxParser
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return request of the operating system name from LinuxParser
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return 0; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return 0; }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }