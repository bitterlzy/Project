#ifndef GLOBALFUNC_H
#define GLOBALFUNC_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <memory>
#include <string.h>
#include <mutex>
#include <unistd.h>

class CGlobalFunc
{
public:
    static CGlobalFunc* GetInstance();
    std::string GetCurrentTime(); 
    std::string GetTimeBySeconds(time_t seconds); 
    time_t GetSecondsByTime(const std::string& time); 
    time_t GetCurrentTimeInSeconds(); 

private:
    CGlobalFunc() = default;
    ~CGlobalFunc() = default;
    CGlobalFunc(const CGlobalFunc&) = delete;
    CGlobalFunc& operator=(const CGlobalFunc&) = delete;
};


#endif