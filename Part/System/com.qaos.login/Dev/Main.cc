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
#include <vector>
#include <filesystem>

#include <termios.h>

#include "Basis.h"
#include "JConf.h"
#include "Kernel.hh"

#include "COM/Login.hh"
#include "COM/User.hh"
#include "COM/SysUI.hh"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;

using namespace qaos;
using namespace com;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
user::sDriver  *User;
sysui::sDriver *SysUI;

login::sDriver DRV = {

  .Start = []()
  {
    vector<pair<idU, string>> Users;

    for (u32 i = 0; i < User->UserC(); i++)
    {
      idU UID = User->UserG(i);
      char* Cac = User->GetName(UID);

      Users.push_back({UID, string(Cac)});

      kernel::dispo(Cac);
    }


    auto PassOK = [&](string UserN, string Pass) -> bool
    {
      for (auto &[UID, Usr]: Users)
        if (Usr == UserN)
          return User->Pass(UID, Pass.c_str());

      return false;
    };

    idU ExitUID = 0;

    

    // Auto Login
    if (fs::exists("/Data/Conf/AutoLogin.conf"))
    {
      jc_stc ALogin = jc_Parse("/Data/Conf/AutoLogin.conf");

      if (auto X = jc_StcGet(ALogin, "Name"); X == Nil)
        goto _l_AL_Err;

      el
      {
        if (char *V = jc_ValGet(X); V == Nil)
          goto _l_AL_Err;
        el
        {
          string UName = string(V);
          jc_DisStr(V);

          for (auto &[UID, Usr]: Users)
            if (Usr == UName)
            {
              jc_DisStc(ALogin);

              ExitUID = UID;
              goto _l_Start;
            }

        }
      }

      _l_AL_Err:
        jc_DisStc(ALogin);
        Log2("AutoLogin failed: Corrupt config file", kernel::lError);
    }


    // Start
    _l_Quest: {
      string User, Pass;
      termios oldt, newt;


      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~ECHO;


      while (true)
      {
        cout << "\033[2J\033[H";

        
        // Prefix
        cout << "Qamio Login\n\n";
        

        // User
        cout << "User Name: ";
        cin >> User;

        // Pass
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        cout << "Password: ";
        cin >> Pass;

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        cout << "\n";


        // Correct ?
        if (PassOK(User, Pass) == true)
        {
          for (auto &[UID, Usr]: Users)
            if (Usr == User)
            {
              ExitUID = UID;
              goto _l_Start;
            }
        }

        else {
          cout << "Incorrect Login\n";
          sleep(2);
        }

      }

    }  


    _l_Start:
      cout << "Hello " << ExitUID << endl;
      SysUI->Start(ExitUID);
  },

};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(login::Domain, &DRV);
  },

  .Pop  = []()
  {
    User = (decltype(User))kernel::NucGet(user::Domain);
    if (User == Nil)
      Log2("Depency: "+ string(user::Domain) +" is not found", kernel::lPanic);


    SysUI = (decltype(SysUI))kernel::NucGet(sysui::Domain);
    if (SysUI == Nil)
      Log2("Depency: "+ string(sysui::Domain) +" is not found", kernel::lPanic);

  },

};
