//------------------------------------------------------------------------------
// File: main.cpp
// Description:  API Entry for DropCache application
// Copyright 2017 Analogic Corp.
//------------------------------------------------------------------------------
#include <unistd.h>
#include <iostream>
#include <fstream>

#define DEFAULT_DROP_WAIT_INTERVAL 120  // Seconds - 2 minutes.

bool g_runQuiet = false;

void doDropCache();

//--------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int sleepInterval = 0;
  if (argc == 1)
  {
    sleepInterval = DEFAULT_DROP_WAIT_INTERVAL;
  }
  if (argc == 2 )
  {
    std::string strInterval = argv[1];
    sleepInterval = std::stoi(strInterval);
  }
  if (argc == 3)
  {
    std::string strInterval = argv[1];
    sleepInterval = std::stoi(strInterval);

    std::string quietStr = argv[2];
    if (quietStr.compare("-q") ==0 )
    {
      g_runQuiet = true;
    }
  }
  if (argc > 3)
  {
    std::cout << "Error in usage - %>DropCache SEC_INTERVAL <-q Silent> \n";
    return -1;
  }

  while(1)
  {
    if (!g_runQuiet)
    {
      std::cout << " *** SLEEPING ***" << std::endl;
    }
    sleep(sleepInterval);
    doDropCache();
  }
  return 0;
}



//------------------------------------------------------------------------------
void doDropCache()
{
  if (!g_runQuiet)
  {
    std::cout << "***** Dropped Cache *********" << std::endl;
  }
  sync();
  std::ofstream ofs("/proc/sys/vm/drop_caches");
  ofs << "3" << std::endl;
}



