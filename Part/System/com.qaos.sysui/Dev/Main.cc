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
#include <cmath>

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
#include "HAL/Input.hh"

#include "FWK/Argon.hh"


using namespace std;
using namespace jix;

using namespace qaos;
using namespace com;
using namespace hal;
using namespace fwk;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Priv
user::sDriver *User;
composer::display::sDriver *Comp_Display;

display::sHAL *Display;
graphic::sHAL *Graphic;
input::sHAL   *Input;


struct bi_form
{
  graphic::poit2_i32 Pos, EPos;
  graphic::size2_u32 Size;

  string Title;
};

vector<bi_form> Forms;



bool n_Accept(idA Aid)
{
  return true;
}

void n_Handler(contact_p Con, data_ Data)
{
  Log("Received data [Size: "+to_string(Data.Size)+"]");



  switch (*(argon::commands*)Data.Point)
  {
  case argon::commands::acNew:
  {
    bi_form AForm;
    argon::WIN_New *Req = (argon::WIN_New*)Data.Point;

    AForm.Pos = {120,120};
    AForm.Size = {Req->W, Req->H};
    AForm.EPos = {AForm.Pos.X +(i32)AForm.Size.W, AForm.Pos.Y +(i32)AForm.Size.H};
    AForm.Title = "FormTest";

    //Forms.push_back(AForm);
    Forms.push_back(AForm);

    Log("Created new argon window");

    break;
  }
  
  default:
    break;
  }

}


void MessageHandler(bool *Started)
{
  contact_p NServer = neon_Server("com.qaos.sysui", 100);

  *Started = true;


  bool Work = true;

  neon_Process(NServer, &Work, &n_Accept, &n_Handler);
}




graphic::poit2_i32 CursorPos = {0,0};

void MHandler_REL(i16 X, i16 Y)
{
  CursorPos.X += X;
  CursorPos.Y += Y;
}

void KHandler_KEY(char *utf8, u32 KeyCode, input::shiftStateSet State)
{
  cout << "Key: " << utf8 << hex << ", KeyCode: " << KeyCode << " , State: " << (u32)State << endl;
}



idP StartApp(string FPath)
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
    char* args[] = {
      (char*)FPath.c_str(),
      Nil,
    };


    if (execvp(args[0], args) == -1) {
      perror("execvp başarısız");
      exit(EXIT_FAILURE);
    }

  } else
  {
    // Çocuğu bekle
    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status))
    {
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
    

    // Start Input
    for (u32 i = 0; i < Input->DRV.Count(); i++)
    {
      auto Cac = Input->DRV.Start(i);
    
      Input->DRV.AddHandlerREL(Cac, &MHandler_REL);
      Input->DRV.AddHandlerKEY(Cac, &KHandler_KEY);
    }


    
    // Run Server
    bool Started = false;
    thread *AThread = new thread(&MessageHandler, &Started);

    while (!Started) {}


    // Start App
    StartApp("/Vendor/Moq/com.jixui.app/Bin/Main.elf");


    // Draw Screen
    while (true)
    {
      auto Buf = Comp_Display->DisplayG(DisSess, 0);

      graphic::size2_i32 Size;
      Graphic->DRV.Get_Size(Buf, &Size);


      Graphic->DRV.Set_Color(Buf, graphic::color(0.22,0.22,0.22));
      Graphic->DRV.Draw_Rect(Buf, {0,0, (f32)Size.W, (f32)Size.H});
      Graphic->DRV.Fill(Buf);

      

      for (auto &X: Forms)
      {
        Graphic->DRV.Set_Color(Buf, graphic::color(1,1,1));
        Graphic->DRV.Draw_RectRound(Buf, {(f32)X.Pos.X, (f32)X.Pos.Y, (f32)X.EPos.X, (f32)X.EPos.Y}, 12);
        Graphic->DRV.Clip(Buf);

        Graphic->DRV.Set_Color(Buf, graphic::color(0.15,0.15,0.15));
        Graphic->DRV.Draw_Rect(Buf, {(f32)X.Pos.X, (f32)X.Pos.Y, (f32)X.EPos.X, (f32)X.EPos.Y});
        Graphic->DRV.Fill(Buf);

        Graphic->DRV.Set_Color(Buf, graphic::color(0.10,0.10,0.10));
        Graphic->DRV.Draw_Rect(Buf, {(f32)X.Pos.X, (f32)X.Pos.Y, (f32)X.EPos.X, (f32)X.Pos.Y +30});
        Graphic->DRV.Fill(Buf);

        Graphic->DRV.Set_FontSize(Buf, 14);

        graphic::size2_f32 TSize;
        Graphic->DRV.Calc_Text(Buf, X.Title.c_str(), &TSize);

        Graphic->DRV.Set_Color(Buf, graphic::color(1,1,1));
        Graphic->DRV.Set_Pos(Buf, {X.Pos.X +((30 -TSize.H) /2), X.Pos.Y +((30 -TSize.H) /2)});
        Graphic->DRV.Draw_Text(Buf, X.Title.c_str());


        {
          const graphic::poit2_f32 O = {(f32)X.EPos.X -15, (f32)X.Pos.Y +15};

          Graphic->DRV.Set_Color(Buf, graphic::color(0.125,0.125,0.125));
          Graphic->DRV.Draw_Arc(Buf, O, 11, 0,M_PI*2);
          Graphic->DRV.Fill(Buf);


          Graphic->DRV.Set_LineSize(Buf, 1);

          Graphic->DRV.Set_Color(Buf, graphic::color(1,1,1));
          Graphic->DRV.Draw_Line(Buf, {O.X -4, O.Y -4}, {O.X +4, O.Y +4});
          Graphic->DRV.Draw_Line(Buf, {O.X -4, O.Y +4}, {O.X +4, O.Y -4});
          Graphic->DRV.Stroke(Buf);

          Graphic->DRV.Set_LineSize(Buf, 2);
        }

        {
          const graphic::poit2_f32 O = {(f32)X.EPos.X -45 +3, (f32)X.Pos.Y +15};

          Graphic->DRV.Set_Color(Buf, graphic::color(0.125,0.125,0.125));
          Graphic->DRV.Draw_Arc(Buf, O, 11, 0,M_PI*2);
          Graphic->DRV.Fill(Buf);

          
          Graphic->DRV.Set_LineSize(Buf, 1);

          Graphic->DRV.Set_Color(Buf, graphic::color(1,1,1));
          Graphic->DRV.Set_Pos(Buf, {O.X -5, O.Y +2});
          Graphic->DRV.Draw_ToLine(Buf, {O.X, O.Y -3});
          Graphic->DRV.Draw_ToLine(Buf, {O.X +5, O.Y +2});
          Graphic->DRV.Stroke(Buf);

          Graphic->DRV.Set_LineSize(Buf, 2);
        }

        {
          const graphic::poit2_f32 O = {(f32)X.EPos.X -75 +6, (f32)X.Pos.Y +15};

          Graphic->DRV.Set_Color(Buf, graphic::color(0.125,0.125,0.125));
          Graphic->DRV.Draw_Arc(Buf, O, 11, 0,M_PI*2);
          Graphic->DRV.Fill(Buf);


          Graphic->DRV.Set_LineSize(Buf, 1);

          Graphic->DRV.Set_Color(Buf, graphic::color(1,1,1));
          Graphic->DRV.Set_Pos(Buf, {O.X -5, O.Y -2});
          Graphic->DRV.Draw_ToLine(Buf, {O.X, O.Y +3});
          Graphic->DRV.Draw_ToLine(Buf, {O.X +5, O.Y -2});
          Graphic->DRV.Stroke(Buf);

          Graphic->DRV.Set_LineSize(Buf, 2);
        }

        Graphic->DRV.Clip_Reset(Buf);
      }


      Graphic->DRV.Set_Color(Buf, graphic::color(1,1,1));
      Graphic->DRV.Draw_Rect(Buf, {(f32)CursorPos.X, (f32)CursorPos.Y, (f32)CursorPos.X +32, (f32)CursorPos.Y +32});
      Graphic->DRV.Fill(Buf);

      

      Graphic->DRV.Flush(Buf);
      Comp_Display->DisplaySwap(DisSess, 0);

      usleep(16);
    }

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


    Input = (decltype(Input))kernel::NucGet(input::Domain);
    if (Input == Nil)
      Log2("Depency: "+ string(input::Domain) +" is not found", kernel::lPanic);

  },

};
