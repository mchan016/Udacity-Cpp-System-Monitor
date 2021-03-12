#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Overload constructor with pid initialized
Process::Process(int pid) : _pid(pid) {}

// Return this process's ID
int Process::Pid() const { return _pid; }

// Calculate this process's CPU utilization
float Process::CpuUtilization() const 
{ 
    long activeJiffs = LinuxParser::ActiveJiffies(_pid);
    float activeTime = static_cast<float>(activeJiffs / sysconf(_SC_CLK_TCK));

    return 100 * activeTime / LinuxParser::UpTime();
}

// Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(_pid); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(_pid); }

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(_pid); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(_pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const 
{
    return CpuUtilization() < a.CpuUtilization() ? true : false; 
}