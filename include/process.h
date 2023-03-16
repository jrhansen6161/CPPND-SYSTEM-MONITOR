#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {
    User();
    Command();
    CpuUtilization();
  }
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_ = 0;
  std::string user_;
  std::string command_;
  float cpuUtilization_ = 0;
  long uptime_ = 0;
  long totaltime_ = 0;
};

#endif