#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
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
    std::string Output= std::to_string(H)+":"+std::to_string(M)+":"+std::to_string(S);
    return Output;
}