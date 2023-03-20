#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T>
T findValueByKey(string const &keyToFind, string const &filePath) {
  string line, key;
  T value;
  std::ifstream filestream(filePath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyToFind) {
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

string findLineByKey(string const &keyToFind, string const &filePath) {
  string line, key;
  std::ifstream filestream(filePath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == keyToFind) {
          filestream.close();
          return line;
        }
      }
    }
  }
  return line;
}

string getFileContents(string const filePath) {
  string line;
  std::ifstream filestream(filePath);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    filestream.close();
  }
  return line;
}

string LinuxParser::OperatingSystem() {
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
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
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

float LinuxParser::MemoryUtilization() {
  float free =
      findValueByKey<float>(kMemFreeKey, kProcDirectory + kMeminfoFilename);
  float total =
      findValueByKey<float>(kMemTotalKey, kProcDirectory + kMeminfoFilename);
  return (total - free) / total;
}

long LinuxParser::UpTime() {
  string line;
  string uptimeString, idleString;
  line = getFileContents(kProcDirectory + kUptimeFilename);
  std::istringstream linestream(line);
  linestream >> uptimeString >> idleString;
  return stol(uptimeString, nullptr, 0);
}

long LinuxParser::Jiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]) + stol(jiffies[CPUStates::kIdle_]) +
         stol(jiffies[CPUStates::kIOwait_]);
}

long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> fields;
  line = getFileContents(kProcDirectory + to_string(pid) + kStatFilename);
  std::istringstream linestream(line);
  while (linestream >> value) {
    fields.push_back(value);
  }
  return stol(fields[ProcessCPUStates::kUtime_]) +
         stol(fields[ProcessCPUStates::kStime_]) +
         stol(fields[ProcessCPUStates::kCutime_]) +
         stol(fields[ProcessCPUStates::kCstime_]);
}

long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]);
}

long LinuxParser::IdleJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key, value;
  vector<string> jiffies;
  line = findLineByKey(kCpuKey, kProcDirectory + kStatFilename);
  std::istringstream linestream(line);
  linestream >> key;
  while (linestream >> value) {
    jiffies.push_back(value);
  }
  return jiffies;
}

int LinuxParser::TotalProcesses() {
  return findValueByKey<int>(kTotalProcessesKey,
                             kProcDirectory + kStatFilename);
}

int LinuxParser::RunningProcesses() {
  return findValueByKey<int>(kProcessesRunningKey,
                             kProcDirectory + kStatFilename);
}

string LinuxParser::Command(int pid) {
  string cmd =
      getFileContents(kProcDirectory + to_string(pid) + kCmdlineFilename);
  return cmd;
}

// Changed VmSize to VmRSS
string LinuxParser::Ram(int pid) {
  return to_string(
      findValueByKey<int>(kProcessRamKey,
                          kProcDirectory + to_string(pid) + kStatusFilename) /
      1024);
}

string LinuxParser::Uid(int pid) {
  return findValueByKey<string>(
      kUidKey, kProcDirectory + to_string(pid) + kStatusFilename);
}

string LinuxParser::User(int pid) {
  string line;
  string user, x, uid;
  string uidToFind = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid == uidToFind) {
        stream.close();
        return user;
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  line = getFileContents(kProcDirectory + to_string(pid) + kStatFilename);
  std::istringstream linestream(line);
  for (int i = 0; i < 22; i++) {
    linestream >> value;
  }
  return LinuxParser::UpTime() - (stol(value) / sysconf(_SC_CLK_TCK));
}
