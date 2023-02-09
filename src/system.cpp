#include "system.h"

#include <algorithm>

#include "linux_parser.h"

Processor& System::Cpu() {
  //  Return the system's CPU
  return cpu_;
}

// Return a container composed of the system's processes
std::vector<Process>& System::Processes() {
  std::vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  // Only add the process if it has information about its RAM usage
  for (const auto& pid : pids) {
    if (!LinuxParser::Ram(pid).empty()) {
      processes_.push_back(Process(pid));
    }
  }
  // Sort the processes_ vector in descending order based on their RAM usage
  sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

std::string System::Kernel() {
  // Return the system's kernel identifier
  return LinuxParser::Kernel();
}

float System::MemoryUtilization() {
  // Return the system's memory utilization
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() {
  // Return the operating system name
  return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() {
  // Return the number of processes actively running on the system
  return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
  // Return the total number of processes on the system
  return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
  // Return the number of seconds since the system started running
  return LinuxParser::UpTime();
  ;
}