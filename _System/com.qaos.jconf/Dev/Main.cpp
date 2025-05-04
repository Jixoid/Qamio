#define ef else if


#include <filesystem>

#include "Basis.hpp"
#include "ParamParser.hh"
#include "JConf.hpp"

using namespace std;


struct {
  i16
    MainVer = 1,
    MajVer  = 0,
    MinVer  = 0,
    PatchVer = 0;

} Ver;




void __Help();
void __Version();
void __Compile();
void __Decompile();


int main(int ArgC, char* ArgV[]) {

  // Parse Params
  ParamParser·Parse(ArgC, ArgV);


  // Choose Mode
  if (ParamParser·Params.Modes.size() == 0)
    __Help();

  elif (ParamParser·Params.Modes[0] == "help" || ParamParser·Params.Modes[0] == "h")
    __Help();

  elif (ParamParser·Params.Modes[0] == "version" || ParamParser·Params.Modes[0] == "v")
    __Version();

  elif (ParamParser·Params.Modes[0] == "compile" || ParamParser·Params.Modes[0] == "c")
    __Compile();

  elif (ParamParser·Params.Modes[0] == "decompile" || ParamParser·Params.Modes[0] == "d")
    __Decompile();

  else {
    cout << "Mode not found: " << ParamParser·Params.Modes[0] << endl;
    exit(1);
  }

  return 0;
}


void __Help() {
  cout << "Mode: Help" << endl;

}

void __Version() {
  cout << "Mode: Version" << endl;

  cout << "v" << Ver.MainVer << "." << Ver.MajVer << "." << Ver.MinVer << ":" << Ver.PatchVer << endl;

}




void __Compile() {
  cout << "Mode: Compile" << endl;

  // Input file
  if (! filesystem::exists(ParamParser·Params.Vars["file"]))
    exit(2);

  
  cJConf Engine;


  cJConf::cStc *Conf = Engine.ParseRaw(ParamParser·Params.Vars["file"]);

  
  Engine.WriteBin(ParamParser·Params.Vars["dest"], Conf);

  delete Conf;
}

void __Decompile() {
  cout << "Mode: Decompile" << endl;

  // Input file
  if (! filesystem::exists(ParamParser·Params.Vars["file"]))
    exit(2);

    
  cJConf Engine;


  cJConf::cStc *Conf = Engine.ParseBin(ParamParser·Params.Vars["file"]);

  
  Engine.WriteRaw(ParamParser·Params.Vars["dest"], Conf);

  delete Conf;
}
