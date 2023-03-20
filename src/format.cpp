#include "format.h"

#include <iomanip>
#include <string>

using std::setfill;
using std::setw;
using std::string;

string Format::ElapsedTime(long seconds) {
  std::ostringstream stream;
  stream << setw(2) << setfill('0') << std::to_string(seconds / 3600) << ":"
         << setw(2) << setfill('0') << std::to_string((seconds % 3600) / 60)
         << ":" << setw(2) << setfill('0')
         << std::to_string((seconds % 3600) % 60);
  return stream.str();
}