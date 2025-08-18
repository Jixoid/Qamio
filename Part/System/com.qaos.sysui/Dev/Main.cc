/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>
#include <format>
#include <thread>

#include <termios.h>
#include <sys/wait.h>

#include "Basis.h"
#include "Kernel.hh"
#include "Neon.h"

#include "COM/SysUI.hh"
#include "COM/User.hh"
#include "COM/Composer/Display.hh"

#include "HAL/Display.hh"
#include "HAL/Graphic.hh"


using namespace std;
using namespace jix;

using namespace qaos;
using namespace com;
using namespace hal;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Priv
user::sDriver *User;
composer::display::sDriver *Comp_Display;

display::sHAL *Display;
graphic::sHAL *Graphic;



bool n_Accept(idA Aid)
{
  return true;
}

void n_Handler(contact_p Con, data_ Data)
{
  Log("Received data [Size: "+to_string(Data.Size)+"]");
}


void MessageHandler(bool *Started)
{
  contact_p NServer = neon_Server("com.qaos.sysui", 100);

  *Started = true;


  bool Work = true;

  neon_Process(NServer, &Work, &n_Accept, &n_Handler);
}





idP StartApp(char* FPath)
{
  pid_t pid = fork();

  if (pid < 0)
  {
    cerr << "fork başarısız!" << endl;
    return 0;
  }

  if (pid == 0)
  {
    // Çocuk işlem (child)
    cout << "Çocuk işlem başlıyor, PID: " << getpid() << endl;

    char* args[] = {
      FPath,
      Nil,
    };


    if (execvp(args[0], args) == -1) {
      perror("execvp başarısız");
      exit(EXIT_FAILURE);
    }

  } else {

    // Ana işlem (parent)
    cout << "Ana işlem, PID: " << getpid() << ", Çocuk PID: " << pid << endl;

    // Çocuğu bekle
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      cout << "Çocuk işlem normal çıktı, çıkış kodu: " << WEXITSTATUS(status) << endl;
    } else {
      cout << "Çocuk işlem beklenmedik şekilde sonlandı." << endl;
    }
  }

  return pid;
}


// Global
sysui::sDriver DRV = {

  .Start = [](idU UID)
  {
    // Start Display
    if (Display->DRV.Count() == 0)
      Log2("No screen found", kernel::lWarn);

    
    auto DisSess = Comp_Display->New(UID);

    for (u32 i = 0; i < Display->DRV.Count(); i++)
    {
      auto Cac = Display->DRV.Start(i);
      
      if (! Comp_Display->AddDisplay(DisSess, Cac))
      {
        Display->DRV.DisConnect(Cac);
        Display->DRV.Stop(Cac);
        Log2(format("Could not connect to Display[{}]", i), kernel::lError);;
      }

    }
    

    // Draw Screen
    auto Buf = Comp_Display->DisplayG(DisSess, 0);

    auto Size = Graphic->DRV.SizeGet(Buf);

    Graphic->DRV.Set_Source_RGB(Buf, graphic::color(0.22,0.22,0.22));
    Graphic->DRV.Basis_Rect(Buf, graphic::rect2d{0,0, Size.W, Size.H});
    Graphic->DRV.Fill(Buf);

    Graphic->DRV.Flush(Buf);
    Comp_Display->DisplaySwap(DisSess, 0);
    

    
    // Run Server
    bool Started = false;
    thread *AThread = new thread(&MessageHandler, &Started);

    while (!Started) {}


    // Start App
    StartApp("/Vendor/Moq/com.jixui.app/Bin/Main.elf");
  

    sleep(3600);
  },

};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(sysui::Domain, &DRV);
  },

  .Pop  = []()
  {
    User = (decltype(User))kernel::NucGet(user::Domain);
    if (User == Nil)
      Log2("Depency: "+ string(user::Domain) +" is not found", kernel::lPanic);


    Comp_Display = (decltype(Comp_Display))kernel::NucGet(composer::display::Domain);
    if (Comp_Display == Nil)
      Log2("Depency: "+ string(composer::display::Domain) +" is not found", kernel::lPanic);


    Display = (decltype(Display))kernel::NucGet(display::Domain);
    if (Display == Nil)
      Log2("Depency: "+ string(display::Domain) +" is not found", kernel::lPanic);


    Graphic = (decltype(Graphic))kernel::NucGet(graphic::Domain);
    if (Graphic == Nil)
      Log2("Depency: "+ string(graphic::Domain) +" is not found", kernel::lPanic);

  },

};
