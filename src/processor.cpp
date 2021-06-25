#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float x=LinuxParser::ActiveJiffies();
    float y=LinuxParser::IdleJiffies();
    float Utilization =y / x;
    return Utilization;
}