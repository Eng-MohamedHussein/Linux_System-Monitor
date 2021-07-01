#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

// Reading and returing the operating system
std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
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

// Reading and returning the kernel name from the filesystem
std::string LinuxParser::Kernel() {
  std::string os, kernel, version;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Reading the Pids of the current Pids of the system
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Reading and returning the system memory utilization
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

// Reading and returning the system uptime value in seconds
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

// Reading and returning the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long Jiffies=0;
  Jiffies=UpTime()*(sysconf(_SC_CLK_TCK));
  return Jiffies;
}

//Reading and returning the number of active jiffies for a PID
//The file contains to strings at the second and third position.
//I'm taking these out first at line just before the loop then the values will be stored in a vector.
//the result is that the 14th element in the file is now the 10th in the vector v
float LinuxParser::ActiveJiffies(int pid) { 
  std::string line;
  long temp;
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

// Reading and returning the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long TotalJiffies=0;
  long Activejiffies=0;
  long Idle= IdleJiffies();
  std::vector<std::string> v= CpuUtilization();
  for (int i=0;i<10;++i){
    TotalJiffies += std::stol(v[i]);
  }
  Activejiffies= TotalJiffies-Idle;
  return Activejiffies;

}

// Reading and returning the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long IdleJiffies=std::stol(CpuUtilization()[3])+std::stol(CpuUtilization()[4]);
  return IdleJiffies;
}

// Reading and returning CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() { 
  std::string line,key;
  std::vector<std::string> v{10};
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

// Reading and returning the total number of processes
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

// Reading and returning the number of running processes
int LinuxParser::RunningProcesses() { 
  std::string line;
  std::string key;
  int RunningProcesses=0;
  int value;
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

// Reading and returning the command of each process
std::string LinuxParser::Command(int pid) { 
  std::string line="";
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream,line);
  }
  return line;
}

// Reading and returing the Ram usage of each process
std::string LinuxParser::Ram(int pid) { 
  std::string line;
  std::string key;
  long int value;
  int Ram;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream>>key;
      if(key=="VmData:"){
        linestream>>value;
      }
    }
  }
  Ram=value/1000;
  return std::to_string(Ram); 
}

// Reading and returing the user ID of each process
std::string LinuxParser::Uid(int pid) { 
  std::string line;
  std::string key;
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

// Reading and returning the user of each pid
std::string LinuxParser::User(int pid) { 
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


//The file contains to strings at theh second and third position.
//I'm, taking these out first just before the loop.
//the result is that the 22th element in the file is now the 19th.
long LinuxParser::UpTime(int pid) {
  std::string line;
  std:: string temp;
  long value;
  long ProcessTime;
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
