#include "process.h"

#include <string>

#include "linux_parser.h"

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
std::string Process::Command() const { return command_; }

// Returns the memory utilization of the process.
std::string Process::Ram() const { return std::to_string(ram_); }

// Returns the memory utilization of the process as an integer.
int Process::getRam() const { return ram_; }

// Returns the user (name) that generated the process.
std::string Process::User() const { return user_; }

// Returns the age of the process (in seconds).
long int Process::UpTime() const { return uptime_; }

// Overloads the "less than" comparison operator for Process objects.
// Used for sorting processes by their CPU utilization.
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}