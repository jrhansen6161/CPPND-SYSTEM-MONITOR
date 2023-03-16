#include "processor.h"
#include "linux_parser.h"
#include <iostream>

float Processor::Utilization() { 
    long active, idle;
    float utilization;
    active = LinuxParser::ActiveJiffies();
    idle = LinuxParser::IdleJiffies();
    utilization = 1.0 * (active - active_) / (active - active_ + idle - idle_);
    active_ = active;
    idle_ = idle;
    return utilization;
}