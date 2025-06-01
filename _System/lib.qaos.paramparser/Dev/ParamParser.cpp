#define ef  else if

#include <iostream>
#include <vector>
#include <regex>


#include "Basis.hpp"

#include "ParamParser.hpp"



namespace jix::ParamParser
{  

  void Parse(int ArgC, char *ArgV[], int ModC, mod *ModV, int &FileC, char **FileV)
  {
    mod *NCon  = ModV;
    int  NConC = ModC;
    
    mod *NMod;



    // GetMode
    for (int i = 0; i < ArgC; ++i)
    {
      string Cac(ArgV[i]);

      if (Cac.at(0) != ':')
        continue;

      Cac.erase(0,1);


      for (int j = 0; j < NConC; j++)
        if (NCon[j].Name == Cac)
        {
          NMod = &NCon[j];

          NCon  = NCon[j].Mods;
          NConC = NCon[j].ModC;

          goto _l_NextMod;
        }

      throw runtime_error("Invalid mod: " +Cac);

      _l_NextMod:
    }



    // GetPars
    for (int i = 0; i < ArgC; i++)
    {
      string Cac(ArgV[i]);
      bool Def = true;

      if (Cac.at(0) != '-')
        continue;

      Cac.erase(0,1);

      if (Cac.at(0) == '-')
      {
        Def = false;
        Cac.erase(0,1);
      }



      for (int j = 0; j < NMod->ParamC; j++)
        if (NMod->Params[j].Name == Cac)
        {
          NMod->Params[j].Def = Def;

          //WriteLn('per: ' +Cac, ' = ', Def);

          goto _l_NextPar;
        }

      throw runtime_error("Unknown param: " +Cac);

      _l_NextPar:
    }


    FileC = 0;
    FileV = Nil;

    // File
    for (int i = 0; i < ArgC; ++i)
    {
      string Cac(ArgV[i]);

      if (Cac.at(0) == ':')
        continue;

      ef (Cac.at(0) == '-')
        continue;

      
      FileC += 1;

      FileV = (char**)realloc(FileV, FileC * sizeof(char*));

      FileV[FileC-1] = ArgV[i];
    }



    NMod->Method(); 
  }

}