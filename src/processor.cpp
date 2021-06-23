#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float Utilization= (LinuxParser::ActiveJiffies()-LinuxParser::IdleJiffies())/LinuxParser::ActiveJiffies();
    return Utilization;
}