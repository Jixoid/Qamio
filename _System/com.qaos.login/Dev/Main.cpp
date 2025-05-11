#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <iomanip>

#include <string.h>
#include <wait.h>
#include <termios.h>
#include <openssl/sha.h>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "JConf.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)


struct form
{
  cWButton *But;
};



struct __loginSess: sessBase
{
  bool OK = false;
  string User = "", Pass = "";
};


struct __loginSess_Graphic: __loginSess
{
  cScreenSess *ScrSess;
  cWindowSess *WinSess;

  cWVisual *Root;

  // UI
  form Form;
};


struct __loginSess_Console: __loginSess
{

};





struct sUser
{
  string Name;
  string Pass;
};

unordered_map<string, sUser> Users;



void GetUserList()
{
  JConf::cStc *Conf = JConf::ParseRaw("/Data/Conf/User.conf");


  for (auto &Obj: Conf->GetStc()["Users"]->GetArr())
  {
    Users[Obj->GetStc()["User"]->GetVal()] =
    {
      .Name = Obj->GetStc()["Name"]->GetVal(),
      .Pass = Obj->GetStc()["Pass"]->GetVal(),
    };
  }

  delete Conf;
}


bool PassOK(string nUser, string nPass)
{
  // Users Exists
  if (Users.find(nUser) == Users.end())
    return false;
  
  
  // Pass
  string FPass = Users[nUser].Pass;
  string HPass;


  _l_Hash:{
    u8 hash[SHA256_DIGEST_LENGTH];

    SHA256((u8*)nPass.c_str(), nPass.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
      ss << hex << setw(2) << setfill('0') << (int)hash[i];
    
    HPass = ss.str();
  }


  // Compare
  return (HPass == FPass);
}



void JString_Dis(jstring* Str)
{
  delete Str->Str;

  delete Str;
}

jstring* JString_New(string Str)
{
  jstring *Ret = new jstring();
  Ret->Dis = &JString_Dis;
  
  Ret->Str = strdup(Str.c_str());

  return Ret;
}



#pragma region Publish

#define Session ((__loginSess_Graphic*)__Session)

void  loginSess_Stop_Graphic(sessBase* __Session)
{
  delete Session->Form.But;

  delete Session->Root;
  delete Session->WinSess;
  delete Session->ScrSess;
  

  //Log2("LoginSess: " +to_string((uPtr)__Session) +", ScrSess: " +to_string((uPtr)Session->ScrSess), lIEnd);
  
  delete Session;
}

#undef Session


loginSess_Graphic  loginSess_Start_Graphic(screenSess ScrSession)
{
  __loginSess_Graphic *Ret = new __loginSess_Graphic();
  Ret->Stop = &loginSess_Stop_Graphic;


  //Log2("LoginSess: " +to_string((uPtr)Ret) +", ScrSess: " +to_string((uPtr)ScrSession), lIBeg);


  // Prepare
  Ret->ScrSess = new cScreenSess(ScrSession);
  Ret->WinSess = new cWindowSess(*Ret->ScrSess);
  
  Ret->Root = new cWVisual(Ret->WinSess->Root());
  

  size2d Size = Ret->Root->Size();


  // Design
  Ret->Form.But = new cWButton();
  Ret->Form.But->Parent(*Ret->Root);
  

  Ret->Form.But->Point({20,20});
  Ret->Form.But->Size({100,30});
  Ret->Form.But->Text("Hello, JixUI!");


  // Return
  return (loginSess_Graphic)Ret;
}





#define Session ((__loginSess_Graphic*)__Session)

void  loginSess_Stop_Console(sessBase *__Session)
{
  Log2("LoginSess: " +to_string((uPtr)__Session), lIEnd);
  
  delete Session;
}

#undef Session


loginSess_Console  loginSess_Start_Console()
{
  __loginSess_Console *Ret = new __loginSess_Console();
  Ret->Stop = &loginSess_Stop_Console;


  Log2("LoginSess: " +to_string((uPtr)Ret), lIBeg);


  // Prepare
  string User, Pass;

  termios oldt, newt;


  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~ECHO;



  while (true)
  {
    cout << "\033[2J\033[H";

    
    // Prefix
    cout << "Qamio Login\n";
    cout << "\n";

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
      break;

    else {
      cout << "Incorrect Login\n";
      sleep(2);
    }

  }
  

  Ret->OK = true;
  Ret->User = User;
  Ret->Pass = Pass;


  // Return
  return (loginSess_Graphic)Ret;
}





#define Session ((__loginSess*)__Session)

bool loginSess_PassOk(loginSess __Session)
{
  return Session->OK;
}

jstring* loginSess_GetUser(loginSess __Session)
{
  return JString_New(Session->User);
}

jstring* loginSess_GetPass(loginSess __Session)
{
  return JString_New(Session->Pass);
}

#undef Session

#pragma endregion






// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Login)
  {
    .Start_Graphic = &loginSess_Start_Graphic,
    .Start_Console = &loginSess_Start_Console,

    .PassOk  = &loginSess_PassOk,
    .GetUser = &loginSess_GetUser,
    .GetPass = &loginSess_GetPass,
  };
}

void Pop()
{
  PopNuc(Graphic)
  PopNuc(Screen);
  PopNuc(Widget);
  PopNuc(Window);
}


void Load()
{
  GetUserList();
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,

  .Load = &Load,
};

#pragma endregion
