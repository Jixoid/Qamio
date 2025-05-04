#include <iostream>
#include <fstream>
#include <vector>
#include <dlfcn.h>
#include <wait.h>

#include "Basis.hpp"
#include "Nucleol.hpp"
#include "JConf.hpp"

#include "Modules.hpp"

using namespace std;
using namespace jix;

#define Log(X)    logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)




inline void TermUp()
{
  cout << "\033[1A";
}


void cNucMng::GetNucList() { for (string Part: {"System", "Vendor", "Product"})
{
  string FPath = "/" +Part+ "/Conf/Nucleol.conf";


  try {
    JConf::cStc *Conf = JConf::ParseRaw(FPath);
    for (auto &Item: Conf->GetStc()["Nucs"]->GetArr())
    {
      sNucleol Nuc;

      Nuc.Package  = Item->GetStc()["Pkg"]->GetVal();
      Nuc.Part     = Part;

      Nucleols.push_back(Nuc);
    }
    delete Conf;
  }
  catch (exception &e) {
    Log2(FPath, lFatal);
  }

  Log(FPath);
}}


void cNucMng::LoadModules() { for (auto &Pkg: Nucleols)
{
  Log2(Pkg.Package, lIBeg);

  // Load
  Pkg.Handle = (point)dlopen(("/" +Pkg.Part+ "/Moq/" +Pkg.Package+ "/Nuc/Main.qo").c_str(), RTLD_NOW);

  if (Pkg.Handle == Nil)
    Log2(dlerror(), lFatal);


  Pkg.Funcs = (sNucStd*)dlsym(Pkg.Handle, "NucStd");

  if (Pkg.Funcs == Nil)
    Log2("NucStd not found", lFatal);


  Log2(Pkg.Package, lIEnd);
}}


void cNucMng::Check() { for (auto &Pkg: Nucleols)
{
  if (Pkg.Funcs->Ver != NucVer)
    Log2("Not Compatible", lFatal);
}}


void cNucMng::Push(sNucCom Com) { for (auto &Pkg: Nucleols)  if (Pkg.Funcs->Push != Nil)
{
  Log2(Pkg.Package, lIBeg);

  try {
    Pkg.Funcs->Push(Com);
  }
  catch(exception &e) {
    Log2("Func is not worked", lFatal);
  }

  Log2(Pkg.Package, lIEnd);
}}

void cNucMng::Pop() { for (auto &Pkg: Nucleols)  if (Pkg.Funcs->Pop != Nil)
{
  Log2(Pkg.Package, lIBeg);

  try {
    Pkg.Funcs->Pop();
  }
  catch(exception &e) {
    Log2("Func is not worked", lFatal);
  }
  
  Log2(Pkg.Package, lIEnd);
}}


void cNucMng::Load() { for (auto &Pkg: Nucleols)  if (Pkg.Funcs->Load != Nil)
{
  Log2(Pkg.Package, lIBeg);

  try {
    Pkg.Funcs->Load();
  }
  catch(exception &e) {
    Log2("Func is not worked", lFatal);
  }

  Log2(Pkg.Package, lIEnd);
}}

void cNucMng::Unload() { for (auto &Pkg: Nucleols)  if (Pkg.Funcs->Unload != Nil)
{
  Log2(Pkg.Package, lIBeg);

  try {
    Pkg.Funcs->Unload();
  }
  catch(exception &e) {
    Log2("Func is not worked", lFatal);
  }

  Log2(Pkg.Package, lIEnd);
}}


void cNucMng::Start() { for (auto &Pkg: Nucleols)  if (Pkg.Funcs->Start != Nil)
{
  Log2(Pkg.Package, lIBeg);

  try {
    Pkg.Funcs->Start();
  }
  catch(exception &e) {
    Log2("Func is not worked", lFatal);
  }

  Log2(Pkg.Package, lIEnd);
}}

void cNucMng::Stop() { for (auto &Pkg: Nucleols)  if (Pkg.Funcs->Stop != Nil)
{
  Log2(Pkg.Package, lIBeg);

  try {
    Pkg.Funcs->Stop();
  }
  catch(exception &e) {
    Log2("Func is not worked", lFatal);
  }

  Log2(Pkg.Package, lIEnd);
}}

