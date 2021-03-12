#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Parse Linux OS name info (/etc/os-release)
string LinuxParser::OperatingSystem() 
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Parse Linux Kernel info (/proc/version)
string LinuxParser::Kernel() 
{
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() 
{
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Parse and return the system memory utilization (/proc/meminfo)
float LinuxParser::MemoryUtilization() 
{
  string category, memTotal, memFree;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> category;
      if (category == "MemTotal")
        lineStream >> memTotal;
      else if (category == "MemFree")
        lineStream >> memFree;
    }
  }

  return (std::stoul(memTotal) - std::stoul(memFree)) / std::stof(memTotal);
}

// Parse and return the system uptime (in seconds)
long LinuxParser::UpTime() 
{ 
  long totalUptime, idleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open())
  {
    getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> totalUptime >> idleTime;
  }

  return totalUptime;
}

// Get process CPU utilization stat, looking for:
// 14. utime | 15. stime | 16. cutime | 17. cstime | 22. starttime
float LinuxParser::ProcCpuUtil(int pid)
{
  const size_t lastElement{22};
  long utime, stime, cutime, cstime, starttime;
  float procUtil;
  string line;
  string kPid = std::to_string(pid) + '/';
  std::ifstream stream(kProcDirectory + kPid + kStatFilename);

  if (stream.is_open())
  {
    getline(stream, line);
    std::istringstream linestream(line);

    // Get values for each field
    for (size_t index = 0; index < lastElement; index++)
    {
      switch (index)
      {
        case 14:
          linestream >> utime;
          break;
        case 15:
          linestream >> stime;
          break;
        case 16:
          linestream >> cutime;
          break;
        case 17:
          linestream >> cstime;
          break;
        default:
          linestream >> starttime;
          break;
      }
    }

    // Calculate CPU usage percentage
    long totalTime = utime + stime + cutime + cstime;
    procUtil = ((static_cast<float>(totalTime) / sysconf(_SC_CLK_TCK)) / UpTime(pid));
  }

  return procUtil;
}

// Fill up the input vector with number of jiffies
void LinuxParser::FillJiffies(std::vector<long>& jiffs, std::istringstream& filestream)
{
  filestream >> jiffs[CPUStates::kUser_];
  filestream >> jiffs[CPUStates::kNice_];
  filestream >> jiffs[CPUStates::kSystem_];
  filestream >> jiffs[CPUStates::kIdle_];
  filestream >> jiffs[CPUStates::kIOwait_];
  filestream >> jiffs[CPUStates::kIRQ_];
  filestream >> jiffs[CPUStates::kSoftIRQ_];
  filestream >> jiffs[CPUStates::kSteal_];
  filestream >> jiffs[CPUStates::kGuest_];
  filestream >> jiffs[CPUStates::kGuestNice_];
}

// Calculate the total active jiffies
long LinuxParser::ActiveJiffies(const vector<long>& jiffs)
{
  return jiffs[CPUStates::kUser_] + jiffs[CPUStates::kNice_] + 
         jiffs[CPUStates::kSystem_] + jiffs[CPUStates::kIRQ_] + 
         jiffs[CPUStates::kSoftIRQ_] + jiffs[CPUStates::kSteal_];
}

// Calculate the total idle jiffies
long LinuxParser::IdleJiffies(const vector<long>& jiffs)
{
  return jiffs[CPUStates::kIdle_] + jiffs[CPUStates::kIOwait_];
}

// Read and return CPU utilization for total and individual CPUs
vector<float> LinuxParser::CpuUtilization() 
{
  const size_t cpuElements{10}; // Total of ten fields
  vector<float> cpus;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line, category;

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::istringstream filestream(line);
      filestream >> category;
      if (category.find("cpu") != string::npos)
      {
        // Get all jiffies data
        std::vector<long> filledJiffies(cpuElements, 0);
        FillJiffies(filledJiffies, filestream);

        long active = ActiveJiffies(filledJiffies);
        long idle = IdleJiffies(filledJiffies);

        cpus.push_back(static_cast<float>(active) / (active + idle));

        continue;
      }

      break;
    }
  }

  return cpus;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  size_t totalProcesses;
  string line, fieldName;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::istringstream lineStream(line);
      lineStream >> fieldName;
      if (fieldName == "processes")
      {
        lineStream >> totalProcesses;
        break;
      }
    }
  }

  return totalProcesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{
  size_t runningProcesses;
  string line, fieldName;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::istringstream lineStream(line);
      lineStream >> fieldName;
      if (fieldName == "procs_running")
      {
        lineStream >> runningProcesses;
        break;
      }
    }
  }

  return runningProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 
  string commandline;
  string kPid = std::to_string(pid) + '/';
  std::ifstream stream(kProcDirectory + kPid + kCmdlineFilename);

  if (stream.is_open())
    getline(stream, commandline);

  return commandline; 
}

// Read and return the memory used by a process (in mB)
string LinuxParser::Ram(int pid) 
{ 
  string line, category;
  size_t ram;
  const size_t kBtomB{1000};  // Ratio for converting kB to mB
  string kPid = std::to_string(pid) + '/';
  std::ifstream stream(kProcDirectory + kPid + kStatusFilename);

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::istringstream lineStream(line);
      lineStream >> category >> ram;
      if (category == "VmSize:")
        break;
    }
  }

  return std::to_string(ram / kBtomB); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{ 
  string line, category;
  string userId;
  string kPid = std::to_string(pid) + '/';
  std::ifstream stream(kProcDirectory + kPid + kStatusFilename);

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::istringstream lineStream(line);
      lineStream >> category;

      if (category == "Uid:")
      {
        lineStream >> userId;
        break;
      }
    }
  }

  return userId;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{ 
  string userId = Uid(pid);
  string line, userName, x, uid;
  std::ifstream stream(kPasswordPath);

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> userName >> x >> uid;
      if (uid == userId)
        break;
    }
  }

  return userName;
}

// Read and return the uptime of a process 
// (22nd item of /proc/[pid]/stat)
long LinuxParser::UpTime(int pid) 
{ 
  string line, startTime;
  const size_t startTimePos{22};
  string kPid = std::to_string(pid) + '/';
  std::ifstream stream(kProcDirectory + kPid + kStatFilename);

  if (stream.is_open())
  {
    getline(stream, line);
    std::istringstream lineStream(line);
    for (size_t index = 0; index < startTimePos; index++)
      lineStream >> startTime;
  }

  // process starttime is the "time the process started after system boot"
  // To get process uptime, convert to seconds and subtract from
  // total system uptime
  return UpTime() - (std::stoul(startTime) / sysconf(_SC_CLK_TCK));
}
