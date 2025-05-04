#define elif  else if 

#include <iostream>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <regex>
#include <string.h>

#include "Basis.hpp"
#include "JConf.hpp"

using namespace std;



namespace jix::JConf {

  namespace {
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

  
  const char *_c_Magic = "Jix.Conf";
  

  string unquote(const string &Str)
  {    
    if (Str.size() >= 2 && Str.front() == '"' && Str.back() == '"') {
      return Str.substr(1, Str.size() -2);
    }

    return Str;
  }

  // Convert
  string cObj::GetVal() {

    if (auto X = dynamic_cast<cVal*>(this))
      return unquote(X->Value);

    else
      throw domain_error("JConf: Object is not of cVal type");
      
  }

  unordered_map<string, cObj*> cObj::GetStc() {

    if (auto X = dynamic_cast<cStc*>(this))
      return X->Objs;

    else
      throw domain_error("JConf: Object is not of cStc type");
      
  }

  vector<cObj*> cObj::GetArr() {

    if (auto X = dynamic_cast<cArr*>(this))
      return X->Objs;

    else
      throw domain_error("JConf: Object is not of cArr type");
      
  }




  inline
  eToken GetType(string Key) {

    if (Key[0] == '"')
      return eToken::Str;

    if (isdigit(Key[0]))
      return eToken::Int;

    for (const auto &Delim: Parsers)
      if (Key.compare(0, Delim.size(), Delim) == 0)
        return eToken::Sym;

    
    return eToken::Txt;
  }



  #pragma region Raw Write 
  
  void __WriteRaw_Stc(ofstream &Stream, cStc *Content, string Prefix);


  void __WriteRaw_Val(ofstream &Stream, cVal *Content) {

    Stream << Content->Value << "," "\n";
  }

  void __WriteRaw_Arr(ofstream &Stream, cArr *Content, string Prefix = "") {

    for (auto &Obj: Content->Objs) {

      Stream << Prefix;


      if (auto X = dynamic_cast<cVal*>(Obj))
        __WriteRaw_Val(Stream, X);

      elif (auto X = dynamic_cast<cArr*>(Obj)){
        Stream << "[" "\n";
        
        __WriteRaw_Arr(Stream, X, Prefix +"  ");

        Stream << Prefix << "]," "\n";
      }

      elif (auto X = dynamic_cast<cStc*>(Obj)) {
        Stream << "{" "\n";
        
        __WriteRaw_Stc(Stream, X, Prefix +"  ");

        Stream << Prefix << "}," "\n";
      }

    }

  }

  void __WriteRaw_Stc(ofstream &Stream, cStc *Content, string Prefix = "") {

    for (auto &[Key, Obj]: Content->Objs) {

      Stream << Prefix << Key << ": ";


      if (auto X = dynamic_cast<cVal*>(Obj); X)
        __WriteRaw_Val(Stream, X);

      elif (auto X = dynamic_cast<cArr*>(Obj); X) {
        Stream << "[" "\n";
        
        __WriteRaw_Arr(Stream, X, Prefix +"  ");

        Stream << Prefix << "]," "\n";
      }

      elif (auto X = dynamic_cast<cStc*>(Obj); X) {
        Stream << "{" "\n";
        
        __WriteRaw_Stc(Stream, X, Prefix +"  ");

        Stream << Prefix << "}," "\n";
      }

    }

  }

  #pragma endregion


  cStc* ParseRaw(string FPath) {

    vector<string> Tokens;

    #pragma region Parse File

    ifstream File(FPath);
    if (!File.is_open())
      throw runtime_error("File not opened: " +FPath);


    string Temp;
    bool InString = false; 


    string Line;
    while (getline(File, Line)) {

      if (! Temp.empty() && ! InString) {
        Tokens.push_back(Temp); // Önceki kelimeyi ekle
        Temp.clear();
      }

      if (InString)
        Temp += "\n";

      // Line Scan
      for (uPtr x = 0; x < Line.size(); ++x) {
        
        // String
        if (Line[x] == '"')
          InString = ! InString;

        if (InString) {
          Temp += Line[x];

          continue;
        }

        
        // Space
        if (Line[x] == ' ') {
          if (! Temp.empty()) {
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
          if (Line.compare(x, Delim.size(), Delim) == 0) {
            if (! Temp.empty()) {

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

    if (! Temp.empty()) {
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
    while (! isEnd) {

      string Name;

      _l_Start:

      // Exit Array
      if (dynamic_cast<cArr*>(Cov) && Word[0] == ']') {

        // Change root
        Chain.pop_back();
        Cov = Chain.back();

        Next();

        goto _l_Start;
      }


      if (dynamic_cast<cArr*>(Cov)) {
        Name = "";
        goto _l_Content;
      } 


      // Exit Struct
      if (dynamic_cast<cStc*>(Cov) && Word[0] == '}') {

        // Change root
        Chain.pop_back();
        Cov = Chain.back();

        Next();

        goto _l_Start;
      }
      

      
      // Name
      if (GetType(Word) != eToken::Txt)
        throw runtime_error("Invalid identifier: \""+ Word +"\" ");


      Name = Word;

      Next();

      // :
      if (Word != ":")
        throw runtime_error("Unexpected word: \""+ Word +"\"");

        Next();




      _l_Content:


      // Content
      if (GetType(Word) == eToken::Str || GetType(Word) == eToken::Int) {

        cVal *Val = new cVal();

        Val->Value = Word;
        

        if (auto X = dynamic_cast<cArr*>(Cov))
          X->Objs.push_back(Val);
        elif (auto X = dynamic_cast<cStc*>(Cov))
          X->Objs[Name] = Val;

        Next();
      }

      // Struct
      elif (Word[0] == '{') {

        cStc *Stc = new cStc();
        
        
        if (auto X = dynamic_cast<cArr*>(Cov))
          X->Objs.push_back(Stc);
        elif (auto X = dynamic_cast<cStc*>(Cov))
          X->Objs[Name] = Stc;

          
        // Change root
        Cov = Stc;
        Chain.push_back(Cov);

        Next();
      }

      // Array
      elif (Word[0] == '[') {

        cArr *Arr = new cArr();
        
        if (auto X = dynamic_cast<cArr*>(Cov))
          X->Objs.push_back(Arr);
        elif (auto X = dynamic_cast<cStc*>(Cov))
          X->Objs[Name] = Arr;


        // Change root
        Cov = Arr;
        Chain.push_back(Cov);

        Next();
      }
      else
        throw runtime_error("Invalid Value: \""+ Word +"\"");

      
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

  void  WriteRaw(string FPath, cStc *Data) {

    ofstream File(FPath);

    File << "# JixConf File" "\n\n";


    __WriteRaw_Stc(File, Data);

    File.close();
  }



  #pragma region Binary Write 
  
  void __WriteBin_Stc(ofstream &Stream, cStc *Content, bool NoMagic = false);


  void __WriteBin_Val(ofstream &Stream, cVal *Content) {

    // Magic
    Stream.write((c8*)(&_c_Val), sizeof(i8));


    // Save Value
    i16 Tmp = Content->Value.size();

    Stream.write((c8*)(&Tmp), sizeof(i16));
    Stream.write(&Content->Value[0], Tmp);
  }

  void __WriteBin_Arr(ofstream &Stream, cArr *Content) {

    // Magic
    Stream.write((c8*)(&_c_Arr), sizeof(i8));

    // Size
    i16 Tmp = Content->Objs.size();

    Stream.write((c8*)(&Tmp), sizeof(i16));


    // Write Sub
    for (auto &Obj: Content->Objs) {


      if (auto X = dynamic_cast<cVal*>(Obj))
        __WriteBin_Val(Stream, X);

      elif (auto X = dynamic_cast<cArr*>(Obj))
        __WriteBin_Arr(Stream, X);

      elif (auto X = dynamic_cast<cStc*>(Obj))
        __WriteBin_Stc(Stream, X);

    }

  }

  void __WriteBin_Stc(ofstream &Stream, cStc *Content, bool NoMagic) {

    // Magic
    if (! NoMagic)
      Stream.write((c8*)(&_c_Stc), sizeof(i8));

    // Size
    i16 Tmp = Content->Objs.size();

    Stream.write((c8*)(&Tmp), sizeof(i16));


    // Write Sub
    for (auto &[Key, Obj]: Content->Objs) {

      // Name
      Tmp = Key.size();
      Stream.write((c8*)(&Tmp), sizeof(i16));
      Stream.write(&Key[0], Tmp);
      

      // Content
      if (auto X = dynamic_cast<cVal*>(Obj))
        __WriteBin_Val(Stream, X);

      elif (auto X = dynamic_cast<cArr*>(Obj))
        __WriteBin_Arr(Stream, X);

      elif (auto X = dynamic_cast<cStc*>(Obj))
        __WriteBin_Stc(Stream, X);

    }

  }

  #pragma endregion

  #pragma region Binary Read 
  
  cStc* __ParseBin_Stc(ifstream &Stream);


  cVal* __ParseBin_Val(ifstream &Stream) {

    // Ret
    cVal *Ret = new cVal();


    // Load Value
    i16 Tmp;

    Stream.read((c8*)(&Tmp), sizeof(i16));
    Ret->Value.resize(Tmp);
    Stream.read(&Ret->Value[0], Tmp);

    // Ret
    return Ret;
  }

  cArr* __ParseBin_Arr(ifstream &Stream) {

    // Ret
    cArr *Ret = new cArr();

    // Size
    i16 Child;
    Stream.read((c8*)(&Child), sizeof(i16));


    // Read Sub
    for (int i = 0; i < Child; i++) {

      // Read Magic
      i16 Tmp;
      Stream.read((c8*)(&Tmp), sizeof(i8));


      // Select Reader
      switch (Tmp)
      {
        case _c_Val:
          Ret->Objs.push_back(
            __ParseBin_Val(Stream)
          );
        break;

        case _c_Arr:
          Ret->Objs.push_back(
            __ParseBin_Arr(Stream)
          );
        break;

        case _c_Stc:
          Ret->Objs.push_back(
            __ParseBin_Stc(Stream)
          );
        break;
      }

    }
    
    // Ret
    return Ret;
  }

  cStc* __ParseBin_Stc(ifstream &Stream) {

    // Ret
    cStc *Ret = new cStc();

    // Size
    i16 Child;
    Stream.read((c8*)(&Child), sizeof(i16));


    // Read Sub
    for (int i = 0; i < Child; i++) {

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
          Ret->Objs[Key] = __ParseBin_Val(Stream);
        break;

        case _c_Arr:
          Ret->Objs[Key] = __ParseBin_Arr(Stream);
        break;

        case _c_Stc:
          Ret->Objs[Key] = __ParseBin_Stc(Stream);
        break;
      }

    }
    
    // Ret
    return Ret;
  }

  #pragma endregion


  cStc* ParseBin(string FPath) {

    ifstream File(FPath, ios::binary);
    if (!File.is_open())
      throw runtime_error("File not opened: " +FPath);
      

    // Magic
    c8 Buf[8];

    File.read(&Buf[0], 8);

    if (memcmp(&Buf[0], _c_Magic, 8) != 0)
      throw runtime_error("Invalid magic");
    


    // Read      
    cStc *Ret =  __ParseBin_Stc(File);

    File.close();


    // Ret
    return Ret;
  }

  void  WriteBin(string FPath, cStc *Data) {

    ofstream File(FPath, ios::binary);

    // Magic
    File.write(_c_Magic, 8);


    __WriteBin_Stc(File, Data, true);

    File.close();
  }

}