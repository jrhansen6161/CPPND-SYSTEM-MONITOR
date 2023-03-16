#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  long totaltime = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  long uptime = LinuxParser::UpTime(pid_);
  if (uptime != 0) {
    cpuUtilization_ = 1.0 * (totaltime - totaltime_) / (uptime - uptime_);
  } else {
    cpuUtilization_ = 0.0;
  }
  return cpuUtilization_;
}

string Process::Command() {
  if (command_.empty()) {
    command_ = LinuxParser::Command(pid_);
  }
  return command_;
}

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() {
  if (user_.empty()) {
    user_ = LinuxParser::User(pid_);
  }
  return user_;
}

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return a.cpuUtilization_ < cpuUtilization_;
}