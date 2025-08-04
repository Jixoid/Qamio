/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

#include "Basis.h"


using namespace std;
using namespace jix;


namespace qaos::kernel
{
  // Nucleol Struct
  inline const u32 NucVer = 4;


  enum nucFlag: u32
  {
    // End: (1 << 31)
  };


  struct nucStd
  {
    // Meta
    u32 Ver = 0;
    u32 Flag = 0;  /* with nucFlag */

    // Load / Unload
    void (*Load)  () = Nil;
    void (*Unload)() = Nil;

    // Start / Stop
    void (*Start)() = Nil;
    void (*Stop) () = Nil;

    // Register
    void (*Push)() = Nil;
    void (*Pop) () = Nil;

    void (*Push_Drv)() = Nil;
  };


  // Nucleol
  void  NucReg(const char* Domain, point Data);
  point NucGet(const char* Domain);

  void  NucDetach(const char* _Module);
  

  // Log
  void Panic(const char* Msg);

  enum logLevel: u8
  {
    lInfo   = 0,
    lIBeg   = 1,
    lIEnd   = 2,
    lDebug  = 3,
    lWarn   = 4,
    lError  = 5,
    lPanic  = 6,
  };

  namespace {
    const string logLevels[] =
    {
      "\033[32;1mINFO \033[0m",
      "\033[32;1mINFO>\033[0m",
      "\033[32;1mINFO<\033[0m",
      "\033[33;1mDEBUG\033[0m",
      "\033[33;1mWARN \033[0m",
      "\033[31;1mERROR\033[0m",
      "\033[31;1mPANIC\033[0m",
    };
  }

  inline void __attribute__((visibility("hidden"))) Logger(const char* Func, string Msg, logLevel Level = logLevel::lInfo)
  {
    string Cac = logLevels[Level] +" "
      +"{"+to_string(getpid())+","+to_string(gettid())+"} "
      +"\033[33;1m@" Module "\033[0m "
      +"\033[30;1m("+Func+")\033[0m "
      +Msg +"\n";

    _l_Write:{
      int fd = open("/Data/Log/Qamio.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
      if (fd < 0)
        Panic("Log file not opened");


      if (flock(fd, LOCK_EX) != 0)
      {
        close(fd);
        Panic("Log file not opened");
      }


      // Log >> File
      write(fd, Cac.c_str(), Cac.size());
      
      // Log >> StdLog
      if (Level != logLevel::lPanic)
        clog << Cac;


      flock(fd, LOCK_UN);
      close(fd);
    }
    sync();


    if (Level == logLevel::lPanic)
      Panic(Cac.c_str());
  }


  // Memory
  point alloc(uPtr Size);
  point rlloc(uPtr Size, point Point);
  void  dispo(point Point);

}
