#define ef else if


#include <filesystem>

#include "Basis.hpp"

#include "ParamParser.hpp"
#include "JConf.hpp"

using namespace std;
using namespace jix;



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



int    FileC;
char **FileV;

int ModC = 1;
ParamParser::mod Mods[] =
{
  {
    .Name = "help\n",
    .Method = &__Help,

    .Params = {},
    .ParamC = 0,

    .Mods = {},
    .ModC = 0,
  }
};


int main(int ArgC, char* ArgV[])
{
  // Parse Params
  ParamParser::Parse(ArgC, ArgV, ModC, Mods, FileC, FileV);

  return 0;
}


void __Help()
{
  cout << "Mode: Help" << endl;
}

void __Version()
{
  cout << "Mode: Version" << endl;

  cout << "v" << Ver.MainVer << "." << Ver.MajVer << "." << Ver.MinVer << ":" << Ver.PatchVer << endl;
}




void __Compile()
{
  cout << "Mode: Compile" << endl;

  // Input file
  if (FileC != 2)
  {
    cerr << "Invalid file parametemers" << endl;
    exit(2);
  }

  if (! filesystem::exists(string(FileV[0])))
    exit(2);

  

  JConf::cStc *Conf = JConf::ParseRaw(string(FileV[0]));

  JConf::WriteBin(string(FileV[1]), Conf);


  delete Conf;
}

void __Decompile()
{
  cout << "Mode: Decompile" << endl;

  // Input file
  if (FileC != 2)
  {
    cerr << "Invalid file parametemers" << endl;
    exit(2);
  }

  if (! filesystem::exists(string(FileV[0])))
    exit(2);

    

  JConf::cStc *Conf = JConf::ParseBin(string(FileV[0]));

  JConf::WriteRaw(string(FileV[1]), Conf);


  delete Conf;
}
