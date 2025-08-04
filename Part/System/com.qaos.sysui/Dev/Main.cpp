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

#include <termios.h>

#include "Basis.h"
#include "Kernel.hpp"

#include "COM/SysUI.hpp"
#include "COM/User.hpp"
#include "COM/Composer/Display.hpp"

#include "HAL/Display.hpp"
#include "HAL/Graphic.hpp"


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



// Global
sysui::sDriver DRV = {

  .Start = [](idU UID)
  {
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

    auto Buf = Comp_Display->DisplayG(DisSess, 0);

    auto Size = Graphic->DRV.SizeGet(Buf);

    Log("Draw");
    Graphic->DRV.Set_Source_RGB(Buf, graphic::color(0.22,0.22,0.22));
    Graphic->DRV.Basis_Rect(Buf, graphic::rect2d{0,0, Size.W, Size.H});
    Graphic->DRV.Fill(Buf);

    Graphic->DRV.Set_Source_RGB(Buf, graphic::color(0.12, 0.12, 0.12));
    Graphic->DRV.Basis_RectR(Buf, graphic::rect2d{40,40, 400, 300}, 8);
    Graphic->DRV.Fill(Buf);

    Graphic->DRV.Set_Source_RGB(Buf, graphic::color(0.45, 0.30, 0.80));
    Graphic->DRV.Basis_RectR(Buf, graphic::rect2d{40,40, 400, 40}, 8);
    Graphic->DRV.Fill(Buf);

    Graphic->DRV.Set_Source_RGB(Buf, graphic::color(1,1,1));
    Graphic->DRV.Basis_Move(Buf, graphic::point2d{40+9,40+9});
    Graphic->DRV.Set_Font_Size(Buf, 15);
    Graphic->DRV.Text(Buf, "Hello Qamio");

    Log("Graphic.Flush");
    Graphic->DRV.Flush(Buf);
    Comp_Display->DisplaySwap(DisSess, 0);
    

    // Wait
    sleep(5);
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
