#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  for (int pid : pids) {
    Process process(pid);
    processes_.push_back(process);
  }
  sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() {
  if (kernel_.empty()) {
    kernel_ = LinuxParser::Kernel();
  }
  return kernel_;
}

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {
  if (operatingsystem_.empty()) {
    operatingsystem_ = LinuxParser::OperatingSystem();
  }
  return operatingsystem_;
}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
