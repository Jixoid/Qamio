/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <dlfcn.h>
#include <wait.h>

#include "Basis.h"
#include "JConf.h"
#include "Kernel.hpp"

#include "Modules.hpp"

using namespace std;
using namespace jix;

using namespace qaos;
namespace fs = std::filesystem;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)




inline void TermUp()
{
  cout << "\033[1A";
}


void cNucMng::GetNucList() { for (string Part: {"System", "Vendor", "Product"})
{
  
  for (const auto &Entry: fs::directory_iterator("/" +Part+ "/Moq"))
    if (Entry.is_directory())
    {
      string
        Pkg  = Entry.path().filename().string(),
        Path = Entry.path().string();

      
      jc_stc Conf = jc_Parse((Path +"/Package.conf").c_str());
      {
        if (Conf == Nil)
        {
          Log2(Part+"::"+Pkg +"/Package.conf is not found", kernel::lWarn);
          goto _l_Close;
        }

        
        jc_arr Kind = jc_StcGet(Conf, "Kind");
        if (Kind == Nil)
        {
          Log2(Part+"::"+Pkg +"/Package.conf -> Kind is not found", kernel::lWarn);
          goto _l_Close;
        }
        else if (!jc_IsArr(Kind))
        {
          Log2(Part+"::"+Pkg +"/Package.conf -> Kind is not compatible", kernel::lWarn);
          goto _l_Close;
        }


        for (u32 i = 0; i < jc_ArrC(Kind); i++)
        {
          jc_val Obj = jc_ArrGet(Kind, i);
          if (!jc_IsVal(Obj))
          {
            Log2(Part+"::"+Pkg +"/Package.conf -> Kind["+to_string(i)+"] is not compatible", kernel::lWarn);
            continue;
          }


          char *CStr = jc_ValGet(Obj);
          string Str = string(CStr);
          jc_DisStr(CStr);

          if (Str == "Nuc")
            goto _l_OK;
        }
      }
      _l_Close: {jc_DisStc(Conf);}
      continue;

      _l_OK: 
      jc_DisStc(Conf);
      Nucleols.push_back(sNucleol{
        .Package = Pkg,
        .Part = Part,
        .Detached = false,
      });
    }

}}


void cNucMng::LoadModules() { for (auto &Pkg: Nucleols)
{
  Log2(Pkg.Part+"::"+Pkg.Package, kernel::lIBeg);

  // Load
  Pkg.Handle = (point)dlopen(("/" +Pkg.Part+ "/Moq/" +Pkg.Package+ "/Nuc/Main.qo").c_str(), RTLD_NOW );

  if (Pkg.Handle == Nil)
    Log2(dlerror(), kernel::lPanic);


  Pkg.Funcs = (kernel::nucStd*)dlsym(Pkg.Handle, "NucStd");

  if (Pkg.Funcs == Nil)
    Log2("NucStd not found", kernel::lPanic);


  Log2(Pkg.Part+"::"+Pkg.Package, kernel::lIEnd);
}}

void cNucMng::UnloadModules() { for (auto &Pkg: Nucleols)
{
  Log2(Pkg.Part+"::"+Pkg.Package, kernel::lIBeg);

  if (dlclose(Pkg.Handle) != 0)
    Log2(dlerror(), kernel::lPanic);

  Log2(Pkg.Part+"::"+Pkg.Package, kernel::lIEnd);
}}


void cNucMng::Check() { for (auto &Pkg: Nucleols)
{
  if (Pkg.Funcs->Ver != kernel::NucVer)
    Log2("Not Compatible: " +Pkg.Package, kernel::lPanic);
}}


void cNucMng::Push() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Push != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Push();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }

  Log2(Pkg.Package, kernel::lIEnd);
}}

void cNucMng::Pop() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Pop != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Pop();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }
  
  Log2(Pkg.Package, kernel::lIEnd);
}}

void cNucMng::Push_Drv() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Push_Drv != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Push_Drv();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }

  Log2(Pkg.Package, kernel::lIEnd);
}}



void cNucMng::Load() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Load != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Load();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }

  Log2(Pkg.Package, kernel::lIEnd);
}}

void cNucMng::Unload() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Unload != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Unload();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }

  Log2(Pkg.Package, kernel::lIEnd);
}}


void cNucMng::Start() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Start != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Start();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }

  Log2(Pkg.Package, kernel::lIEnd);
}}

void cNucMng::Stop() { for (auto &Pkg: Nucleols)  if (!Pkg.Detached && Pkg.Funcs->Stop != Nil)
{
  Log2(Pkg.Package, kernel::lIBeg);

  try {
    Pkg.Funcs->Stop();
  }
  catch(exception &e) {
    Log2("Func is not worked", kernel::lPanic);
  }

  Log2(Pkg.Package, kernel::lIEnd);
}}

