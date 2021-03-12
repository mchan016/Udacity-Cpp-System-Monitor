#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization() const;                  // TODO: See src/process.cpp
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

  
 private:
  int _pid;
};

#endif