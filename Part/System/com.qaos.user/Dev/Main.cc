/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <sstream>
#include <iomanip>

#include <openssl/sha.h>

#include "Basis.h"
#include "JConf.h"
#include "Kernel.hh"

#include "COM/User.hh"


using namespace std;
using namespace jix;

using namespace com;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Priv
struct __user
{
  idU UID;
  string Name, Nick, Pass;
};

vector<__user> Users;

void _Reset()
{
  Users.clear();

  jc_obj Conf = jc_Parse("/Data/Conf/User.conf");
  if (!jc_IsStc(Conf))
    Log2("Corrupt config file: /Data/Conf/User.conf", kernel::lPanic);

  
  jc_arr CUsers = jc_StcGet(Conf, "Users");
  if (!jc_IsArr(CUsers))
    Log2("Corrupt config file: /Data/Conf/User.conf -> Users", kernel::lPanic);


  for (u32 i = 0; i < jc_ArrC(CUsers); i++)
  {
    jc_obj Cac;
    char *CStr;

    jc_stc Item = jc_ArrGet(CUsers, i);
    if (!jc_IsStc(Item))
      Log2("Corrupt config file: /Data/Conf/User.conf -> Users["+to_string(i)+"]", kernel::lPanic);


    __user U;

    // .UID
    Cac = jc_StcGet(Item, "UID");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /Data/Conf/User.conf -> Users["+to_string(i)+"].UID", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    U.UID = stoul(string(CStr));
    jc_DisStr(CStr);


    // .User
    Cac = jc_StcGet(Item, "User");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /Data/Conf/User.conf -> Users["+to_string(i)+"].User", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    U.Name = string(CStr);
    jc_DisStr(CStr);


    // .Nick
    Cac = jc_StcGet(Item, "Nick");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /Data/Conf/User.conf -> Users["+to_string(i)+"].Nick", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    U.Nick = string(CStr);
    jc_DisStr(CStr);


    // .Pass
    Cac = jc_StcGet(Item, "Pass");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /Data/Conf/User.conf -> Users["+to_string(i)+"].Pass", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    U.Pass = string(CStr);
    jc_DisStr(CStr);


    Users.push_back(U);
  }

};


// Global
user::sDriver DRV = {

  .UserC = []() -> u32
  {
    return Users.size();
  },

  .UserG = [](u32 Index) -> idU
  {
    if (Index >= Users.size())
      return 0;

    return Users[Index].UID;
  },


  .GetName = [](idU UID) -> char*
  {
    for (auto &X: Users)
      if (X.UID == UID)
      {
        char *Ret = (char*)kernel::alloc(X.Name.size()+1);
        memcpy(Ret, X.Name.c_str(), X.Name.size()+1);
        return Ret;
      }

    return Nil;
  },

  .GetNick = [](idU UID) -> char*
  {
    for (auto &X: Users)
      if (X.UID == UID)
      {
        char *Ret = (char*)kernel::alloc(X.Nick.size()+1);
        memcpy(Ret, X.Nick.c_str(), X.Nick.size()+1);
        return Ret;
      }

    return Nil;
  },


  .Pass = [](idU UID, const char* Key) -> bool
  {
    string FPass;

    for (auto &X: Users)
      if (X.UID == UID)
      {
        FPass = X.Pass;
        goto _l_Hash;
      }
    
    return false;


    // Hash
    _l_Hash:
      string HPass;

      // Create Hash
      u8 hash[SHA256_DIGEST_LENGTH];

      SHA256((u8*)Key, strlen(Key), hash);

      stringstream ss;
      for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
      
      HPass = ss.str();


      // Compare
      return (HPass == FPass);
  },
  
};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Load = []()
  {
    _Reset();
  },

  .Push = []()
  {
    kernel::NucReg(user::Domain, &DRV);
  },
  
};
