#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    //long utilization=LinuxParser::ActiveJiffies()/LinuxParser::ActiveJiffies()+LinuxParser::IdleJiffies();
    long x=LinuxParser::ActiveJiffies();
    long y=LinuxParser::IdleJiffies();
    long z= x+y;
    float utilization= (float) x / z;
    
    
    return utilization;
}