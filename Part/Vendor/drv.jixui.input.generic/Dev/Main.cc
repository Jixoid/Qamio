/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <cmath>
#include <format>
#include <vector>
#include <thread>
#include <filesystem>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Input.hh"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <xkbcommon/xkbcommon.h>


using namespace std;
using namespace jix;
using namespace hal;

using namespace qaos;

namespace fs = std::filesystem;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



struct __input;

struct __inputSess
{
  __input *Input;

  bool Valid;

  int Handle;

  bool Active = false;
  thread *Thread = Nil;

  vector<input::handlerREL> IH_REL;
  vector<input::handlerWHL> IH_WHL;
  vector<input::handlerABS> IH_ABS;
  vector<input::handlerKEY> IH_KEY;

  u32 A_X, A_Y;
};

struct __input
{
  string Path;

  __inputSess *Sess = Nil;
};

vector<__input> OpenInput;



struct xkb_context *Ctx = Nil;
struct xkb_keymap *Keymap = Nil;
struct xkb_state *State = Nil;
bool Control = false;

char KeyBuf[8];


void xkb_translate(u32 Code, int Pressed, input::handlerKEY Handler)
{
  memset(&KeyBuf[0], 0, sizeof(KeyBuf));
  
  // Utf 8
  int N = xkb_state_key_get_utf8(State, Code +8, KeyBuf, sizeof(KeyBuf));

  // Get Sym
  auto KSym = xkb_state_key_get_one_sym(State, Code +8);

  // Shift update
  if (KSym == XKB_KEY_Control_L || KSym == XKB_KEY_Control_R)
    Control = (Pressed == 1);
  el
    xkb_state_update_key(State, Code +8, (Pressed == 1) ? XKB_KEY_DOWN:XKB_KEY_UP);


  // Shift prepare
  input::shiftStateSet RState = 0;
  
  if (Control)
    RState |= input::shiftStates::ssCtrl;

  if (xkb_state_mod_index_is_active(State, xkb_keymap_mod_get_index(Keymap, XKB_MOD_NAME_SHIFT), XKB_STATE_MODS_DEPRESSED))
    RState |= input::shiftStates::ssShift;

  if (xkb_state_mod_index_is_active(State, xkb_keymap_mod_get_index(Keymap, XKB_MOD_NAME_ALT), XKB_STATE_MODS_DEPRESSED))
    RState |= input::shiftStates::ssAlt;

  if (xkb_state_mod_index_is_active(State, xkb_keymap_mod_get_index(Keymap, XKB_VMOD_NAME_SUPER), XKB_STATE_MODS_DEPRESSED))
    RState |= input::shiftStates::ssSuper;

  if (xkb_state_mod_index_is_active(State, xkb_keymap_mod_get_index(Keymap, XKB_VMOD_NAME_LEVEL3), XKB_STATE_MODS_DEPRESSED))
    RState |= input::shiftStates::ssAltGr;
  
  Handler(&KeyBuf[0], KSym, RState);
}


void Handler(__inputSess *Session) { while (Session->Active)
{
  //lock_guard Lock(Session->Wait);
  

  input_event Ev;

  ssize_t Bytes = read(Session->Handle, &Ev, sizeof(Ev));
  
  if (Bytes != sizeof(Ev))
    continue;


  // Keypad
  if (Ev.type == EV_KEY)
  {
    if (Session->IH_KEY.size() != 0)
      for (auto &X: Session->IH_KEY)
        xkb_translate(Ev.code, Ev.value, X);
  }


  // Mouse
  ef (Ev.type == EV_REL)
  {
    // Related //
    if (Session->IH_REL.size() != 0)
    {
      if (Ev.code == REL_X)
        for (auto &X: Session->IH_REL)
          X(Ev.value, 0);
      
      ef (Ev.code == REL_Y)
        for (auto &X: Session->IH_REL)
          X(0, Ev.value);
    }


    // Whell //
    if (Session->IH_WHL.size() != 0)
    {
      if (Ev.code == REL_WHEEL)
        for (auto &X: Session->IH_WHL)
          X(Ev.value, 0);

      ef (Ev.code == REL_HWHEEL)
        for (auto &X: Session->IH_WHL)
          X(0, Ev.value);
    }

  }

  // Tablet
  ef (Ev.type == EV_ABS)
  {
    // Absolute //
    if (Session->IH_ABS.size() != 0)
    {
      if (Ev.code == ABS_X)
        Session->A_X = Ev.value;
      
      ef (Ev.code == ABS_Y)
        Session->A_Y = Ev.value;


      for (auto &X: Session->IH_ABS)
        X(Session->A_X, Session->A_Y);
    }
  }

}}

bool is_device(string Device)
{
  int fd = open(Device.c_str(), O_RDONLY);
  if (fd == -1) 
    return false;

  
  u64 evbit[EV_MAX /64 +1];
  if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evbit) == -1)
    Log2("Unknown error", kernel::lError);


  bool Have_Key = evbit[EV_KEY /64] & (1UL << (EV_KEY %64));
  bool Have_Rel = evbit[EV_REL /64] & (1UL << (EV_REL %64));
  bool Have_Abs = evbit[EV_ABS /64] & (1UL << (EV_ABS %64));

  close(fd);

  return (Have_Key || Have_Rel || Have_Abs);
}

string device_name(string Device)
{
  int fd = open(Device.c_str(), O_RDONLY);
  if (fd == -1)
    return "<unknown>";

  
  char name[256] = "<unknown>";
  if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0)
  {
    close(fd);
    return "<unknown>";
  }

  el
  {
    close(fd);
    return string(name);
  }
}



// Global
input::sDriver DRV = {

  #define Self ((__inputSess*)__Self)

  // General
  .Reset = []()
  {
    for (auto &X: OpenInput)
      if (X.Sess != Nil)
      {
        auto Y = X.Sess;
        X.Sess = Nil;


        // Stop //

        // Valid
        Y->Valid = false;

        // Thread
        Y->Active = false;
        if (Y->Thread->joinable())
          Y->Thread->join();

        // File
        if (Y->Handle != 0)
        {
          close(Y->Handle);
          Y->Handle = 0;
        }

        // Con Device
        Y->Input = Nil;
      }


    for (auto &Entry: fs::directory_iterator("/dev/input"))
      if (Entry.path().string().find("event") != string::npos && is_device(Entry.path().string()))
        OpenInput.push_back({
          .Path = Entry.path().string(),
          .Sess = Nil,
        });

        
    for (auto &X: OpenInput)
      Log("Detected input: " +device_name(X.Path));
        
        
    Log2("Finded input count: " +to_string(OpenInput.size()), kernel::lDebug);
  },

  .Count = []() -> u32
  {
    return OpenInput.size();
  },


  .Start = [](u32 Index) -> point
  {
    // Already ?
    if (OpenInput[Index].Sess != Nil)
      Log2("Device is not suitable", kernel::lPanic);



    // Interface
    __inputSess *Ret = new __inputSess();
    Ret->Input = &OpenInput[Index];
    
    Ret->Input->Sess = Ret;


    // Start
    Ret->Handle = open(Ret->Input->Path.c_str(), O_RDONLY | O_NONBLOCK);
    if (Ret->Handle == -1)
    {
      Log2("Can't open device: " +Ret->Input->Path, kernel::lError);
      return Nil;
    }


    Ret->Valid = true;
    
    Ret->Active = true;
    Ret->Thread = new thread(Handler, Ret);


    // Return
    return (__inputSess*)Ret;
  },

  .Stop = [](point __Self)
  {
    // Stop
    Self->Active = false;
    if (Self->Thread->joinable())
      Self->Thread->join();
    
    delete Self->Thread;
    
    
    if (Self->Handle != 0)
      close(Self->Handle);


    Self->Input->Sess = Nil;

    // Dispose
    delete Self;
  },

  
  .IsValid = [](point __Self) -> bool
  {
    return Self->Valid;
  },


  .AddHandlerREL = [](point __Self, input::handlerREL Handler)
  {
    if (std::find(Self->IH_REL.begin(), Self->IH_REL.end(), Handler) == Self->IH_REL.end())
      Self->IH_REL.push_back(Handler);
  },

  .AddHandlerWHL = [](point __Self, input::handlerWHL Handler)
  {
    if (std::find(Self->IH_WHL.begin(), Self->IH_WHL.end(), Handler) == Self->IH_WHL.end())
      Self->IH_WHL.push_back(Handler);
  },

  .AddHandlerABS = [](point __Self, input::handlerABS Handler)
  {
    if (std::find(Self->IH_ABS.begin(), Self->IH_ABS.end(), Handler) == Self->IH_ABS.end())
      Self->IH_ABS.push_back(Handler);
  },

  .AddHandlerKEY = [](point __Self, input::handlerKEY Handler)
  {
    if (std::find(Self->IH_KEY.begin(), Self->IH_KEY.end(), Handler) == Self->IH_KEY.end())
      Self->IH_KEY.push_back(Handler);
  },


  .DelHandlerREL = [](point __Self, input::handlerREL Handler)
  {
    if (auto X = std::find(Self->IH_REL.begin(), Self->IH_REL.end(), Handler); X != Self->IH_REL.end())
      Self->IH_REL.erase(X);
  },

  .DelHandlerWHL = [](point __Self, input::handlerWHL Handler)
  {
    if (auto X = std::find(Self->IH_WHL.begin(), Self->IH_WHL.end(), Handler); X != Self->IH_WHL.end())
      Self->IH_WHL.erase(X);
  },

  .DelHandlerABS = [](point __Self, input::handlerABS Handler)
  {
    if (auto X = std::find(Self->IH_ABS.begin(), Self->IH_ABS.end(), Handler); X != Self->IH_ABS.end())
      Self->IH_ABS.erase(X);
  },

  .DelHandlerKEY = [](point __Self, input::handlerKEY Handler)
  {
    if (auto X = std::find(Self->IH_KEY.begin(), Self->IH_KEY.end(), Handler); X != Self->IH_KEY.end())
      Self->IH_KEY.erase(X);
  },


  #undef Self
};

input::sinfo INF = {
  .Name = "JixUI generic input driver",
};



// Nuc Control
input::sHAL *HAL = Nil;


extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Load = []()
  {
    setenv("XKB_CONFIG_ROOT", "/System/Moq/lib.qaos.xkb/XKB", 1);

    struct xkb_rule_names Name = {
      .rules = "evdev",
      .model = "pc105",
      .layout = "tr",
      .variant = "",
      .options = Nil,
    };

    Ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    Keymap = xkb_keymap_new_from_names(Ctx, &Name, XKB_KEYMAP_COMPILE_NO_FLAGS);
    State = xkb_state_new(Keymap);

    DRV.Reset();
  },

  .Unload = []()
  {
    if (HAL != Nil)
      HAL->DelDriver(&DRV);

    xkb_state_unref(State);
    xkb_keymap_unref(Keymap);
    xkb_context_unref(Ctx);
  },


  .Pop = []()
  {
    HAL = (decltype(HAL))kernel::NucGet(input::Domain);
    if (HAL == Nil)
    {
      Log2(string(input::Domain) +" is not found", kernel::lWarn);
      Log2("Module self-detach", kernel::lError);

      // Cancel installation
      kernel::NucDetach(Module);

      return;
    }
    
  },

  .Push_Drv = []()
  {
    HAL->RegDriver(&DRV, &INF);
  },

};
