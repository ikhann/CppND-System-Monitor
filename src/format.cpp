#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the f unction
string Format::ElapsedTime(long seconds) {
  // convert seconds into hr, min, sec
  auto hr = seconds / 3600;
  auto min = seconds / 60 % 60;
  auto sec = seconds % 60;
  std::stringstream ss;
  ss << std::setw(2) << std::setfill('0') << hr << ':';
  ss << std::setw(2) << std::setfill('0') << min << ':';
  ss << std::setw(2) << std::setfill('0') << sec;
  return ss.str();
}