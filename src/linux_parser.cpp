#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::string key,line;
  float value,TotalMemory,FreeMemory,UsedMemory;
  std::ifstream stream(kProcDirectory+kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream>>key>>value;
      if (key=="MemTotal:"){
        TotalMemory=value;
      }
      else if(key=="MemFree:"){
        FreeMemory=value;
        break;
      }
    }
  }
  UsedMemory=TotalMemory - FreeMemory;
  return UsedMemory/TotalMemory;
}

// DONE: Read and return the system uptime value in seconds
long LinuxParser::UpTime() { 
  std::string line;
  long Uptime,idle;
  std::ifstream stream(kProcDirectory+kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream>>Uptime>>idle;
  }
  return Uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long Jiffies;
  Jiffies=UpTime()*(sysconf(_SC_CLK_TCK));
  return Jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::ActiveJiffies(int pid) { 
  std::string line,temp;
  long value;
  std::vector<long> v;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream>>  value>>temp>>temp;
    for(int i=0;i<19;++i){
      linestream>>  value;
      v.push_back(value);
    }   
  }
  long utime=v[10],stime=v[11],cutime=v[12],cstime=v[13],starttime=v[18];
  long totaltime=utime+stime+cutime+cstime;

  long elapsed = Jiffies() - starttime;
  float utilization= (float) totaltime/elapsed;
  return utilization;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long TotalJiffies,Activejiffies;
  std::vector<string> v= CpuUtilization();
  for (int i=0;i<10;++i){
    TotalJiffies += std::stol(v[i]);
  }
  Activejiffies= TotalJiffies-IdleJiffies();
  return Activejiffies;

}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long IdleJiffies=std::stol(CpuUtilization()[3])+std::stol(CpuUtilization()[4]);
  return IdleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::string line,key;
  std::vector<string> v{10};
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream>>key;
      if( key == "cpu"){
        linestream>>v[0]>>v[1]>>v[2]>>v[3]>>v[4]>>v[5]>>v[6]>>v[7]>>v[8]>>v[9];
        break;
      }
    }
  }
  return v;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::string line,key;
  int TotalProcesses=0, value;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while (linestream>>key>>value )  
      {
        if(key=="processes"){
          TotalProcesses=value;
        }
      }
    }
  }
  return TotalProcesses;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  std::string line,key;
  int RunningProcesses=0, value;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while (linestream>>key>>value )  
      {
        if(key=="procs_running"){
          RunningProcesses=value;
        }
      }
    }
  }
  return RunningProcesses;
  
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::string line;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream,line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::string line,key;
  long int value;
  int Ram;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream>>key;
      if(key=="VmSize:"){
        linestream>>value;
      }
    }
  }
  Ram=value/1000;
  return std::to_string(Ram); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::string line,key;
  int uid;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if(stream.is_open()){
    while (std::getline(stream,line))
    {
      std::istringstream linestream(line);
      linestream>> key ;
      if (key=="Uid:"){
        linestream>>key>>uid;
        break;
      }
    }
  }
  return std::to_string(uid);
  
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::string line,x,user;
  int value;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::replace(line.begin(),line.end(),':',' ');
      std::istringstream linestream(line);
      linestream>>user>>x>>value;
      if(value==stoi(Uid(pid))){
        break;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::string line,temp;
  long value,ProcessTime;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream>>value>>temp>>temp;
    for(int i=0;i<19;++i){
      linestream>>  value;
    }
  }
  ProcessTime= LinuxParser::UpTime()- (value/sysconf(_SC_CLK_TCK));
  return ProcessTime;
}
