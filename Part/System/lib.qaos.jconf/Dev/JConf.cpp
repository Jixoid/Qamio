/*
  This file is part of JConf
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with JConf. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef  else if 
#define el  else

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <utility> 

#include "Basis.h"
#include "JConf.h"

using namespace std;



// Magic
const char _c_Mag_Jixoid[10] = {'\e','^','J','I','X','0','1','D','!','\a'};

const char _c_Mag_JConf[7] = {'C','o','n','f','i','g','_'};


const u8 _c_Val = 0x1;
const u8 _c_Stc = 0x2;
const u8 _c_Arr = 0x3;


// Intern
struct cObj
{
  u8 Type;
};


struct cVal: cObj
{
  cVal()
  {
    Type = _c_Val;
  }

  string Val;
};

struct cStc: cObj
{
  cStc()
  {
    Type = _c_Stc;
  }

  vector<pair<string, cObj*>> Stc;
};

struct cArr: cObj
{
  cArr()
  {
    Type = _c_Arr;
  }

  vector<cObj*> Arr;
};




#define Obj ((cObj*)__Obj)


// Is
extern "C" bool jc_IsVal(jc_obj __Obj)
{
  return (__Obj != Nil) && (Obj->Type == _c_Val);
}

extern "C" bool jc_IsStc(jc_obj __Obj)
{
  return (__Obj != Nil) && (Obj->Type == _c_Stc);
}

extern "C" bool jc_IsArr(jc_obj __Obj)
{
  return (__Obj != Nil) && (Obj->Type == _c_Arr);
}



// New / Dis
extern "C" jc_val  jc_NewVal()
{
  return new cVal();
}

extern "C" void    jc_DisVal(jc_val __Obj)
{
  delete Obj;
}


extern "C" jc_stc  jc_NewStc()
{
  return new cStc();
}

extern "C" void    jc_DisStc(jc_stc __Obj)
{
  delete Obj;
}


extern "C" jc_arr  jc_NewArr()
{
  return new cArr();
}

extern "C" void    jc_DisArr(jc_arr __Obj)
{
  delete Obj;
}


#undef Obj



#define SVal ((cVal*)__Val)
#define SStc ((cStc*)__Stc)
#define SArr ((cArr*)__Arr)

// Set/Get
extern "C" char*   jc_ValGet(jc_val __Val)
{
  char* Ret;

  size_t Len = strlen(SVal->Val.c_str());
  Ret = (char*)malloc(Len+1);
  if (Ret == Nil)
    return Nil;

  memcpy(Ret, SVal->Val.c_str(), Len+1);

  return Ret;
}

extern "C" bool    jc_ValSet(jc_val __Val, const char* Value)
{
  SVal->Val = string(Value);

  return true;
}

extern "C" void    jc_DisStr(char *Str)
{
  free(Str);
}



extern "C" jc_obj  jc_StcGet(jc_stc __Stc, const char* Name)
{
  for (auto &[Key, Val]: SStc->Stc)
    if (Key == string(Name))
      return Val;

  return Nil;
}

extern "C" bool    jc_StcSet(jc_stc __Stc, const char* Name, jc_obj Object)
{
  for (auto &[Key, Val]: SStc->Stc)
    if (Key == string(Name))
      return Val = (cObj*)Object;


  SStc->Stc.push_back({string(Name), (cObj*)Object});

  return true;
}

extern "C" bool    jc_StcDel(jc_stc __Stc, const char* Name)
{
  SStc->Stc.erase(

    std::remove_if(SStc->Stc.begin(), SStc->Stc.end(),
      [&](const auto &X) {
        return X.first == std::string(Name);
      }
    ),  
    SStc->Stc.end()
  );

  return true;
}

extern "C" u32     jc_StcC  (jc_stc __Stc)
{
  return SStc->Stc.size();
}

extern "C" jc_obj  jc_StcInd(jc_stc __Stc, u32 Index, char** Name)
{
  size_t Len = strlen(SStc->Stc[Index].first.c_str());
  *Name = (char*)malloc(Len+1);
  if (*Name == Nil)
    return Nil;

  memcpy(*Name, SStc->Stc[Index].first.c_str(), Len+1);


  return SStc->Stc[Index].second;
}



extern "C" u32     jc_ArrC   (jc_arr __Arr)
{
  return SArr->Arr.size();
}

extern "C" jc_obj  jc_ArrGet (jc_arr __Arr, u32 Index)
{
  if (Index >= SArr->Arr.size())
    return Nil;

  return SArr->Arr[Index];
}

extern "C" bool    jc_ArrSet (jc_arr __Arr, u32 Index, jc_obj Object)
{
  if (Index >= SArr->Arr.size())
    return false;

  SArr->Arr[Index] = (cObj*)Object;

  return true;
}

extern "C" bool    jc_ArrPush(jc_arr __Arr, jc_obj Object)
{
  SArr->Arr.push_back((cObj*)Object);

  return true;
}

extern "C" bool    jc_ArrDel (jc_arr __Arr, u32 Index)
{
  if (Index >= SArr->Arr.size())
    return false;


  SArr->Arr[Index] = SArr->Arr.back();
  SArr->Arr.pop_back();

  return true;
}

#undef SVal
#undef SStc
#undef SArr


// Parse
namespace
{
  string Parsers[] = {
    " ", "@", ".",":", ",",";",
    "(",")", "[","]", "{","}", "<",">"
  };

  enum eToken {
    Str,
    Int,
    Sym,
    Txt,
  };
}


string unquote(const string &Str)
{    
  if (Str.size() >= 2 && Str.front() == '"' && Str.back() == '"') {
    return Str.substr(1, Str.size() -2);
  }

  return Str;
}

inline eToken GetType(string Key)
{
  if (Key[0] == '"')
    return eToken::Str;

  if (isdigit(Key[0]))
    return eToken::Int;

  for (const auto &Delim: Parsers)
    if (Key.compare(0, Delim.size(), Delim) == 0)
      return eToken::Sym;

  
  return eToken::Txt;
}



// Parse
extern "C" jc_stc jc_Parse(const char* FPath)
{
  bool Bin;

  _l_PreRead: {
    ifstream File(FPath, ios::binary);
    if (!File.is_open())
      return Nil;
      //throw runtime_error("File not opened: " +string(FPath));
      

    // Magic
    char Buf[10+7];
    File.read(&Buf[0], 10+7);
    
    Bin =
      (memcmp(&Buf[00], _c_Mag_Jixoid, 10) == 0) &&
      (memcmp(&Buf[10], _c_Mag_JConf,   7) == 0);
  }


  // Route
  return Bin ? jc_ParseBin(FPath):jc_ParseRaw(FPath);
}


// ParseRaw
extern "C" jc_stc jc_ParseRaw(const char* FPath)
{
  vector<string> Tokens;

  #pragma region Parse File

  ifstream File(FPath);
  if (!File.is_open())
    return Nil;
    //throw runtime_error("File not opened: " +FPath);


  string Temp;
  bool InString = false; 


  string Line;
  while (getline(File, Line))
  {
    if (! Temp.empty() && ! InString)
    {
      Tokens.push_back(Temp); // Önceki kelimeyi ekle
      Temp.clear();
    }

    if (InString)
      Temp += "\n";

    // Line Scan
    for (uPtr x = 0; x < Line.size(); ++x)
    {
      // String
      if (Line[x] == '"')
        InString = ! InString;

      if (InString)
      {
        Temp += Line[x];

        continue;
      }

      
      // Space
      if (Line[x] == ' ')
      {
        if (! Temp.empty())
        {
          Tokens.push_back(Temp); // Önceki kelimeyi ekle
          Temp.clear();
        }
        continue;
      }

      // Comment
      if (Line[x] == '#')
        goto _l_NextLine;
      

      // Others
      for (const auto &Delim: Parsers)
        if (Line.compare(x, Delim.size(), Delim) == 0)
        {
          if (! Temp.empty())
          {

            Tokens.push_back(Temp); // Önceki kelimeyi ekle
            Temp.clear();
          }
          Tokens.push_back(Delim); // Delimiter'ı da ekle
          x += Delim.size() - 1; // Atlama yap

          goto _l_NextChar;
        }
      

      // Add
      Temp += Line[x];
    
      // Next
      _l_NextChar: {}
    }

    _l_NextLine: {}
  }

  if (! Temp.empty())
  {
    Tokens.push_back(Temp); // Önceki kelimeyi ekle
    Temp.clear();
  }


  File.close();    
  #pragma endregion


  // Root
  cStc *Root = new cStc();

  #pragma region Interpret

  #define isEnd  (Step >= Size)
  #define Next()  Step++; if (isEnd) goto _l_Escape;
  #define Word  Tokens[Step]

  uPtr Size = Tokens.size();
  uPtr Step = 0;

  // Root
  cObj *Cov = Root;
  vector<cObj*> Chain = {Root};


  // Start
  while (! isEnd)
  {
    string Name;

    _l_Start:

    // Exit Array
    if (Cov->Type == _c_Arr && Word[0] == ']')
    {
      // Change root
      Chain.pop_back();
      Cov = Chain.back();

      Next();

      goto _l_Start;
    }


    if (Cov->Type == _c_Arr)
    {
      Name = "";
      goto _l_Content;
    } 


    // Exit Struct
    if (Cov->Type == _c_Stc && Word[0] == '}')
    {
      // Change root
      Chain.pop_back();
      Cov = Chain.back();

      Next();

      goto _l_Start;
    }
    

    
    // Name
    if (GetType(Word) != eToken::Txt)
      return Nil;
      //throw runtime_error("Invalid identifier: \""+ Word +"\" ");


    Name = Word;

    Next();

    // :
    if (Word != ":")
      return Nil;
      //throw runtime_error("Unexpected word: \""+ Word +"\"");

      Next();




    _l_Content:


    // Content
    if (auto T = GetType(Word); T == eToken::Str || T == eToken::Int)
    {

      cVal *Val = new cVal();

      if (T == eToken::Str)
        Val->Val = unquote(Word);
      
      el
        Val->Val = Word;


      if (Cov->Type == _c_Arr)
        ((cArr*)Cov)->Arr.push_back(Val);

      ef (Cov->Type == _c_Stc)
        ((cStc*)Cov)->Stc.push_back({Name, Val});

      Next();
    }

    // Struct
    ef (Word[0] == '{')
    {
      cStc *Stc = new cStc();
      
      
      if (Cov->Type == _c_Arr)
        ((cArr*)Cov)->Arr.push_back(Stc);
      
      ef (Cov->Type == _c_Stc)
        ((cStc*)Cov)->Stc.push_back({Name, Stc});

        
      // Change root
      Cov = Stc;
      Chain.push_back(Cov);

      Next();
    }

    // Array
    ef (Word[0] == '[')
    {
      cArr *Arr = new cArr();
      
      if (Cov->Type == _c_Arr)
        ((cArr*)Cov)->Arr.push_back(Arr);

      ef (Cov->Type == _c_Stc)
        ((cStc*)Cov)->Stc.push_back({Name, Arr});


      // Change root
      Cov = Arr;
      Chain.push_back(Cov);

      Next();
    }

    el
      return Nil;
      //throw runtime_error("Invalid Value: \""+ Word +"\"");

    
    // ,
    if (Word[0] == ',')
      Next();
  }

  _l_Escape:


  #undef Word
  #undef Next
  #undef isEnd

  #pragma endregion


  return Root;
}



// ParseBin
  cStc* __ParseBin_Stc(ifstream &Stream);

  cVal* __ParseBin_Val(ifstream &Stream)
  {
    // Ret
    cVal *Ret = new cVal();


    // Load Value
    i16 Tmp;

    Stream.read((c8*)(&Tmp), sizeof(i16));
    Ret->Val.resize(Tmp);
    Stream.read(&Ret->Val[0], Tmp);

    // Ret
    return Ret;
  }

  cArr* __ParseBin_Arr(ifstream &Stream)
  {
    // Ret
    cArr *Ret = new cArr();

    // Size
    i16 Child;
    Stream.read((c8*)(&Child), sizeof(i16));


    // Read Sub
    for (int i = 0; i < Child; i++)
    {

      // Read Magic
      i16 Tmp;
      Stream.read((c8*)(&Tmp), sizeof(i8));


      // Select Reader
      switch (Tmp)
      {
        case _c_Val:
          Ret->Arr.push_back(
            __ParseBin_Val(Stream)
          );
        break;

        case _c_Arr:
          Ret->Arr.push_back(
            __ParseBin_Arr(Stream)
          );
        break;

        case _c_Stc:
          Ret->Arr.push_back(
            __ParseBin_Stc(Stream)
          );
        break;
      }

    }
    
    // Ret
    return Ret;
  }

  cStc* __ParseBin_Stc(ifstream &Stream)
  {
    // Ret
    cStc *Ret = new cStc();

    // Size
    i16 Child;
    Stream.read((c8*)(&Child), sizeof(i16));


    // Read Sub
    for (int i = 0; i < Child; i++)
    {
      // Name
      i16 Tmp;
      string Key;

      Stream.read((c8*)(&Tmp), sizeof(i16));
      Key.resize(Tmp);
      Stream.read(&Key[0], Tmp);
      

      // Read Magic
      Stream.read((c8*)(&Tmp), sizeof(i8));

      // Select Reader
      switch (Tmp)
      {
        case _c_Val:
          Ret->Stc.push_back({Key, __ParseBin_Val(Stream)});
        break;

        case _c_Arr:
          Ret->Stc.push_back({Key, __ParseBin_Arr(Stream)});
        break;

        case _c_Stc:
          Ret->Stc.push_back({Key, __ParseBin_Stc(Stream)});
        break;
      }

    }
    
    // Ret
    return Ret;
  }


extern "C" jc_stc jc_ParseBin(const char* FPath)
{
  ifstream File(FPath, ios::binary);
  if (!File.is_open())
    return Nil;
    //throw runtime_error("File not opened: " +string(FPath));
    

  // Magic
  c8 Buf[10+7];
  File.read(&Buf[0], 10+7);


  if (memcmp(&Buf[00], _c_Mag_Jixoid, 10) != 0)
    return Nil;
    //throw runtime_error("Invalid jixoid magic");

  if (memcmp(&Buf[10], _c_Mag_JConf, 7) != 0)
    return Nil;
    //throw runtime_error("Invalid magic");
  


  // Read      
  cStc *Ret =  __ParseBin_Stc(File);

  File.close();


  // Ret
  return Ret;
}




// WriteRaw
  void __WriteRaw_Stc(ofstream &Stream, cStc *Content, string Prefix);

  void __WriteRaw_Val(ofstream &Stream, cVal *Content)
  {
    Stream << Content->Val << "," "\n";
  }

  void __WriteRaw_Arr(ofstream &Stream, cArr *Content, string Prefix = "")
  {
    for (auto &Objec: Content->Arr)
    {
      Stream << Prefix;


      if (Objec->Type == _c_Val)
      {
        __WriteRaw_Val(Stream, (cVal*)Objec);
      }

      ef (Objec->Type == _c_Arr)
      {
        Stream << "[" "\n";
        
        __WriteRaw_Arr(Stream, (cArr*)Objec, Prefix +"  ");

        Stream << Prefix << "]," "\n";
      }

      ef (Objec->Type == _c_Stc)
      {
        Stream << "{" "\n";
        
        __WriteRaw_Stc(Stream, (cStc*)Objec, Prefix +"  ");

        Stream << Prefix << "}," "\n";
      }

    }

  }

  void __WriteRaw_Stc(ofstream &Stream, cStc *Content, string Prefix = "")
  {
    for (auto &[Key, Objec]: Content->Stc)
    {
      Stream << Prefix << Key << ": ";


      if (Objec->Type == _c_Val)
      {
        __WriteRaw_Val(Stream, (cVal*)Objec);
      }

      ef (Objec->Type == _c_Arr)
      {
        Stream << "[" "\n";
        
        __WriteRaw_Arr(Stream, (cArr*)Objec, Prefix +"  ");

        Stream << Prefix << "]," "\n";
      }

      ef (Objec->Type == _c_Stc)
      {
        Stream << "{" "\n";
        
        __WriteRaw_Stc(Stream, (cStc*)Objec, Prefix +"  ");

        Stream << Prefix << "}," "\n";
      }

    }

  }


extern "C" bool jc_WriteRaw(const char* FPath, jc_stc Data)
{
  ofstream File(FPath);

  File << "# JixConf File" "\n\n";


  __WriteRaw_Stc(File, (cStc*)Data);

  File.close();

  return true;
}



// WriteBin
  void __WriteBin_Stc(ofstream &Stream, cStc *Content, bool NoMagic = false);

  void __WriteBin_Val(ofstream &Stream, cVal *Content)
  {
    // Magic
    Stream.write((c8*)(&_c_Val), sizeof(i8));


    // Save Value
    i16 Tmp = Content->Val.size();

    Stream.write((c8*)(&Tmp), sizeof(i16));
    Stream.write(&Content->Val[0], Tmp);
  }

  void __WriteBin_Arr(ofstream &Stream, cArr *Content)
  {
    // Magic
    Stream.write((c8*)(&_c_Arr), sizeof(i8));

    // Size
    i16 Tmp = Content->Arr.size();

    Stream.write((c8*)(&Tmp), sizeof(i16));


    // Write Sub
    for (auto &Objec: Content->Arr)
    {
      if (Objec->Type == _c_Val)
        __WriteBin_Val(Stream, (cVal*)Objec);

      ef (Objec->Type == _c_Arr)
        __WriteBin_Arr(Stream, (cArr*)Objec);

      ef (Objec->Type == _c_Stc)
        __WriteBin_Stc(Stream, (cStc*)Objec);

    }

  }

  void __WriteBin_Stc(ofstream &Stream, cStc *Content, bool NoMagic)
  {
    // Magic
    if (! NoMagic)
      Stream.write((c8*)(&_c_Stc), sizeof(i8));

    // Size
    i16 Tmp = Content->Stc.size();

    Stream.write((c8*)(&Tmp), sizeof(i16));


    // Write Sub
    for (auto &[Key, Objec]: Content->Stc)
    {
      // Name
      Tmp = Key.size();
      Stream.write((c8*)(&Tmp), sizeof(i16));
      Stream.write(&Key[0], Tmp);
      

      // Content
      if (Objec->Type == _c_Val)
        __WriteBin_Val(Stream, (cVal*)Objec);

      ef (Objec->Type == _c_Arr)
        __WriteBin_Arr(Stream, (cArr*)Objec);

      ef (Objec->Type == _c_Stc)
        __WriteBin_Stc(Stream, (cStc*)Objec);

    }

  }


extern "C" bool jc_WriteBin(const char* FPath, jc_stc Data)
{
  ofstream File(FPath, ios::binary);

  // Magic
  File.write(_c_Mag_Jixoid, 10);
  File.write(_c_Mag_JConf,  7);


  __WriteBin_Stc(File, (cStc*)Data, true);

  File.close();

  return true;
}
