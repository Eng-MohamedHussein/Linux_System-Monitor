#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

// Returning th Cpu Utilization of the system
Processor& System::Cpu() {return cpu_; }

//Filling out the vector Processes_ with the system's processes and returing it
std::vector<Process>& System::Processes() { 
    processes_.clear();
    std::vector<int> pids=LinuxParser::Pids();
    for (int pid : pids){
        
        processes_.emplace_back(pid);
    }

    std::sort(processes_.begin(),processes_.end(), [](Process a, Process b){return a<b;});
    return processes_; 
}

// Returning the system's kernel identifier as string
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Returning the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Returning the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Returning the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Returning the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Returninh the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }