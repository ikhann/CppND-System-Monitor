#include "process.h"
#include "linux_parser.h"
#include <string>

#define MAX_COMMAND_SIZE 40

// Constructor for the Process class. Initializes all member variables.
Process::Process(int pid)
    : pid_(pid),
      user_(LinuxParser::User(pid)),
      command_(LinuxParser::Command(pid)),
      ram_(std::stol(LinuxParser::Ram(pid))),
      uptime_(LinuxParser::UpTime(pid)) {
  // Calculate the CPU utilization of the process.
  long seconds = LinuxParser::UpTime() - uptime_;
  long totaltime = LinuxParser::ActiveJiffies(pid_);
  try {
    utilization_ = static_cast<float>(totaltime) / static_cast<float>(seconds);
  } catch (...) {
    utilization_ = 0;
  }
}

// Returns the process ID.
int Process::Pid() const { return pid_; }

// Returns the CPU utilization of the process.
float Process::CpuUtilization() const { return utilization_; }

// Returns the command that generated the process.
std::string Process::Command() const {
  // Append ... in the end if the command exceeds more than 40 characters
  if (command_.size() > MAX_COMMAND_SIZE) {
    return command_.substr(0, MAX_COMMAND_SIZE) + "...";
  }
  return command_; 
}

// Returns the memory utilization of the process.
std::string Process::Ram() const { return std::to_string(ram_); }

// Returns the memory utilization of the process as an integer.
int Process::getRam() const { return ram_; }

// Returns the user (name) that generated the process.
std::string Process::User() const { return user_; }

// Returns the age of the process (in seconds).
long int Process::UpTime() const { return LinuxParser::UpTime() - uptime_; }

// Overloads the "less than" comparison operator for Process objects.
// Used for sorting processes by their CPU utilization.
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}