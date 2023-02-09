#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  // Get the current values of total, active, and idle jiffies
  auto total_new = LinuxParser::Jiffies();
  auto active_new = LinuxParser::ActiveJiffies();

  // Calculate the delta values of total, active, and idle jiffies since last
  // call
  float total_delta = total_new - total_;
  float active_delta = active_new - active_;

  // Store the current values as the previous values for next call
  total_ = total_new;
  active_ = active_new;

  // Calculate and return the CPU utilization
  return active_delta / total_delta;
}