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
  wButton But;
};



struct __loginSession: qsession
{
  bool OK = false;
  string User = "", Pass = "";
};


struct __loginSession_Graphic: __loginSession
{
  screenSession ScrSession;
  windowSession WinSession;

  wVisual Root;

  // UI
  form Form;
};


struct __loginSession_Console: __loginSession
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

#define Session ((__loginSession_Graphic*)__Session)

void  loginSession_Stop_Graphic(loginSession_Graphic __Session)
{
  Session->Form.But->Dis(Session->Form.But);
  
  Session->WinSession->Stop(Session->WinSession);


  Log2("LoginSess: " +to_string((uPtr)__Session) +", ScrSess: " +to_string((uPtr)Session->ScrSession), lIEnd);
  
  delete Session;
}

#undef Session


loginSession_Graphic  loginSession_Start_Graphic(screenSession ScrSession)
{
  __loginSession_Graphic *Ret = new __loginSession_Graphic();
  Ret->Stop = &loginSession_Stop_Graphic;


  Log2("LoginSess: " +to_string((uPtr)Ret) +", ScrSess: " +to_string((uPtr)ScrSession), lIBeg);


  // Prepare
  Ret->ScrSession = ScrSession;

  Ret->WinSession = Window.Start(Ret->ScrSession);
  
  Ret->Root = Window.Root(Ret->WinSession);
  

  size2d Size = Widget.WVisual.SizeGet(Ret->Root);


  // Design
  Ret->Form.But = Widget.WButton.New();
  Widget.Widget.ParentSet(Ret->Form.But, Ret->Root);


  Widget.WVisual.PointSet(Ret->Form.But, {20, 20});
  Widget.WVisual.SizeSet(Ret->Form.But, {100, 30});
  Widget.WVisual.TextSet(Ret->Form.But, "Hello, JixUI!");


  // Return
  return (loginSession_Graphic)Ret;
}





#define Session ((__loginSession_Graphic*)__Session)

void  loginSession_Stop_Console(loginSession_Console __Session)
{
  Log2("LoginSess: " +to_string((uPtr)__Session), lIEnd);
  
  delete Session;
}

#undef Session


loginSession_Console  loginSession_Start_Console()
{
  __loginSession_Console *Ret = new __loginSession_Console();
  Ret->Stop = &loginSession_Stop_Console;


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
  return (loginSession_Graphic)Ret;
}





#define Session ((__loginSession*)__Session)

bool loginSession_PassOk(loginSession __Session)
{
  return Session->OK;
}

jstring* loginSession_GetUser(loginSession __Session)
{
  return JString_New(Session->User);
}

jstring* loginSession_GetPass(loginSession __Session)
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
    .Start_Graphic = &loginSession_Start_Graphic,
    .Start_Console = &loginSession_Start_Console,

    .PassOk  = &loginSession_PassOk,
    .GetUser = &loginSession_GetUser,
    .GetPass = &loginSession_GetPass,
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
