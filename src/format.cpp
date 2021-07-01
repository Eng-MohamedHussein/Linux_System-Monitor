#include<iomanip>
#include <string>

#include "format.h"

using std::string;


// OUTPUT: HH:MM:SS

string Format::ElapsedTime(long int seconds ) { 
    int H,M,S;
    if((seconds/3600) < 0){
        H=0;
        M=seconds/60;
        S=seconds-(M*60);
    }
    else{
        H=seconds/3600;
        M=(seconds-(H*3600))/60;
        S=seconds-(H*3600+M*60);
    }
    
    std::ostringstream output;
    output<< std::setw(2) << std::setfill('0') << H << ":" 
         << std::setw(2) << std::setfill('0') << M << ":"
         << std::setw(2) << std::setfill('0') << S;
  return output.str();
}