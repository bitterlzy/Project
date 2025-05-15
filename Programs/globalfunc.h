#ifndef GLOBALFUNC_H
#define GLOBALFUNC_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <memory>
#include <string.h>
#include <mutex>
#include <unistd.h>

namespace CGlobalFunc
{
    void sleepForSeconds(int seconds);
    void sleepForMilliseconds(int milliseconds);
    std::string getCurrentTime(); 
    std::string getTimeBySeconds(time_t seconds); 
    time_t getSecondsByTime(const std::string& time); 
    time_t getCurrentTimeInSeconds(); 
};


#endif