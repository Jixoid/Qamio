#define ef  else if

#include <iostream>
#include <vector>
#include <regex>


#include "Basis.hpp"

#include "ParamParser.hpp"



namespace ParamParser
{
  sParams Params;
  

  void Parse(iPtr ArgC, c8* ArgV[])
  {
    regex P_Mode(R"(^/([A-Za-z0-9/]+)$)");
    regex P_Var (R"(^\.([A-Za-z0-9]+):(.+)$)");
    regex P_FAdd(R"(^\+([A-Za-z0-9]+)$)");
    regex P_FSub(R"(^-([A-Za-z0-9]+)$)");

    
    for (iPtr x = 1; x < ArgC; ++x)
    {
      string Input = ArgV[x];
      smatch Match;

      if (regex_match(Input, Match, P_Mode))
      {
        stringstream ss(Match[1].str());
        string Segment;

        while (getline(ss, Segment, '/'))
          if (!Segment.empty())
          Params.Modes.push_back(Segment);

      }
      ef (regex_match(Input, Match, P_Var))
        Params.Vars[Match[1].str()] = Match[2].str();

      ef (regex_match(Input, Match, P_FAdd))
        Params.Vars[Match[1].str()] = "yes";

      ef (regex_match(Input, Match, P_FSub))
        Params.Vars[Match[1].str()] = "";

    }

  }

}