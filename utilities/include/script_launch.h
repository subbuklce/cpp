#pragma once

#include <iostream>
#include <string.h>
#include <stdint.h>
#include <istream>
#include <vector>

class cScript
{

public:
static std::string launch_bash(const char* fullcmd);

//static pid_t launchProcess(std::vector<const char*> argv);
static pid_t proc_find(const char* name);
private:



};



