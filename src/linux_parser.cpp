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
  string category;
  size_t memTotal, memFree;
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

  return (memTotal - memFree) / static_cast<float>(memTotal);
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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

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

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
