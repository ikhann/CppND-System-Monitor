#include "linux_parser.h"
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

using std::ifstream;
using std::istringstream;

// Returns the name of the operating system.
std::string LinuxParser::OperatingSystem() {
  std::string line, key, value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // Replace all spaces, equals signs, and double quotes with underscores
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          // Replace underscores with spaces
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Returns the version of the Linux kernel.
std::string LinuxParser::Kernel() {
  std::string line, os, kernel, version;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Returns a list of PIDs.
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  for (const auto& entry : std::filesystem::directory_iterator(kProcDirectory)) {
    // Check if the file name is a valid PID
    if (entry.is_directory() && std::isdigit(entry.path().filename().string()[0])) {
      int pid = std::stoi(entry.path().filename().string());
      pids.push_back(pid);
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  // Read and return the system memory utilization
  // Calculation: (Total memory - free memory) / Total Memory
  std::string line, key;
  float total = 1, free = 1;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (stream.is_open()) {
    // Read through the /proc/meminfo file line by line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      // Check if the current line is the total memory line
      if (key == "MemTotal:") {
        linestream >> total;
        // Check if the current line is the available memory line
      } else if (key == "MemAvailable:") {
        linestream >> free;
        // Break the loop as we have found what we're looking for
        break;
      }
    }
  }
  // Calculate the memory utilization and return the result
  return (total - free) / total;
}

long LinuxParser::UpTime() {
  // Read and return the system uptime
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    // Read the first line of the /proc/uptime file directly into the uptime variable
    stream >> uptime;
  }
  // Return the uptime value
  return uptime;
}

long LinuxParser::Jiffies() {
  // Return the sum of the active and idle jiffies
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long utime, stime, cutime, cstime; // Variables to store the values of utime, stime, cutime, and cstime
  std::string line;
  std::vector<std::string> values;
  // Open and read the stat file for the given pid
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    // Use a stringstream to extract values from the line
    std::istringstream linestream(line);
    std::string value;
    int i = 0; // Index counter for the values vector
    while (linestream >> value) {
      // Only store the relevant values (utime, stime, cutime, and cstime)
      if (i == 13 || i == 14 || i == 15 || i == 16) {
        values.push_back(value);
      }
      ++i;
    }
  }
  // atol is faster than stol for converting strings to long ints
  utime = std::atol(values[0].c_str());
  stime = std::atol(values[1].c_str());
  cutime = std::atol(values[2].c_str());
  cstime = std::atol(values[3].c_str());
  // Calculate the total active jiffies
  long totaltime = utime + stime + cutime + cstime;
  // Return the result divided by the clock ticks per second
  return totaltime / sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies() {
  // Read and return the number of active jiffies for the system
  auto jiffies = CpuUtilization();

  long user = std::stol(jiffies[CPUStates::kUser_]);
  long nice = std::stol(jiffies[CPUStates::kNice_]);
  long system = std::stol(jiffies[CPUStates::kSystem_]);
  long irq = std::stol(jiffies[CPUStates::kIRQ_]);
  long softirq = std::stol(jiffies[CPUStates::kSoftIRQ_]);
  long steal = std::stol(jiffies[CPUStates::kSteal_]);
  // Return the sum of the different states
  return user + nice + system + irq + softirq + steal;
}

long LinuxParser::IdleJiffies() {
  // Read and return the number of idle jiffies for the system
  auto jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

std::vector<std::string> LinuxParser::CpuUtilization() {
  // Read and return CPU utilization
  std::vector<std::string> jiffies;
  std::string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Skip the first word in the line
    std::string dummy;
    linestream >> dummy;

    // Read the remaining values and add them to the vector
    std::string value;
    while (linestream >> value) {
      jiffies.push_back(value);
    }
  }
  return jiffies;
}

int LinuxParser::TotalProcesses() {
  // Read and return the total number of processes
  int processes = 0;
  std::string key;
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      // Check if the line starts with the key "processes"
      if (line.find("processes") == 0) {
        std::istringstream linestream(line);
        linestream >> key >> processes;
        break;
      }
    }
  }
  return processes;
}

int LinuxParser::RunningProcesses() {
  // Read and return the number of running processes
  // Open the file containing information about the number of running processes
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    // Read the file line by line
    std::string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      std::string key;
      linestream >> key;
      if (key == "procs_running") {
        // If the key is "procs_running", read the value
        int processes;
        linestream >> processes;
        return processes;
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
std::string LinuxParser::Command(int pid) {
  // Open the file containing information about the command of a process
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    // Read the first line of the file
    std::string command;
    std::getline(stream, command);
    return command;
  }
  return "";
}

// Read and return the memory used by a process
std::string LinuxParser::Ram(int pid) {
  std::string line, key;
  long mem;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        // Read the memory used by the process in kilobytes
        linestream >> mem;
        mem /= 1000;
        // Convert the memory used to a string and return it
        return std::to_string(mem);
      }
    }
  }
  return "0";
}

std::string LinuxParser::Uid(int pid) {
  // Read and return the user ID associated with a process
  std::string line, key, uid;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  }
  return uid;
}

std::string LinuxParser::User(int pid) {
  // Read and return the user associated with a process
  std::string uid = Uid(pid);
  std::string line, username, x, userid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      // split the line on the delimiter ':' and extract the necessary fields
      std::istringstream linestream(line);
      std::getline(linestream, username, ':');
      std::getline(linestream, x, ':');
      std::getline(linestream, userid, ':');

      if (userid == uid) {
        // return the username if the uid matches the uid of the process
        return username;
      }
    }
  }
  return "DEFAULT";
}

long LinuxParser::UpTime(int pid) {
  // Read and return the uptime of a process
  std::string line, value;
  long starttime = 0;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    // Get the 22nd value which represents the starttime of the process
    // from the first line of the stat file.
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i) {
      linestream >> value;
    }
    try {
      starttime = stol(value) / sysconf(_SC_CLK_TCK);
    } catch (...) {
      starttime = 0;
    }
  }
  return starttime;
}