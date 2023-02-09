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
  int Pid() const;
  int getRam() const;
  std::string User() const;
  std::string Command() const;
  std::string Ram() const;
  float CpuUtilization() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;

private:
  int pid_;
  std::string user_;
  std::string command_;
  long ram_;
  long uptime_;
  float utilization_;
};

#endif