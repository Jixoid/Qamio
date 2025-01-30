#include <iostream>
#include <fstream>
#include <vector>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Terminal.cpp"
#include "BootAnim.hpp"

extern "C" {
  #include <dlfcn.h>
}

#include "ini.h"

using namespace std;
using namespace jix;
  




class cNucMng {

  protected:

  struct sNucleol {
    string  Package;
    bool    Essential;
    point   Handle;
  };

  vector<sNucleol> Nucleols;


  public:

  void GetNucList() {

    mINI::INIFile file("/Conf/Nucleol.conf");

    mINI::INIStructure ini;
    file.read(ini);


    int ListC = stoi(ini["Root"]["ListC"]);

    for (int i = 1; i <= ListC; i++) {
      sNucleol Nuc;

      Nuc.Package   = ini["Root"]["List_"+to_string(i)];
      Nuc.Essential = ( ini[Nuc.Package]["Essential"] == "1");
      
      Nucleols.push_back(Nuc);
    }

  }
 
  void LoadModules() {

    for (auto &Pkg: Nucleols) {
      
      cout << "[ PEND ] Loading: " << Pkg.Package << endl;

      // Load
      Pkg.Handle = (point)dlopen(("/Pkg/"+Pkg.Package+"/Lib/Main.so").c_str(), RTLD_NOW);

      if (Pkg.Handle == NULL) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        if (Pkg.Essential)
          throw runtime_error( dlerror() );
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, " << dlerror() << endl;
          continue;
        }

      } else { 
        term::Up();
        cout << "[  OK  ]" << endl;
      }

    }

  }


  void Check() {

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;

      
      // Check
      nucCheck NucCheck = (nucCheck)dlsym(Pkg.Handle, "NucCheck");

      if (NucCheck == NULL) {
        cout << "[ FAIL ] Checking: " << Pkg.Package << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucCheck not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucCheck not found" << endl;
          continue;
        }

      } else { 
        try {
          if (!NucCheck(gnucVer))
            throw runtime_error(Pkg.Package+" is not compatible");
        
        }
        catch(exception &e) {
          cout << "[ FAIL ] Checking: " << Pkg.Package << endl;

          if (Pkg.Essential)
            throw runtime_error("NucCheck not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucCheck not worked: " << e.what() << endl;
            continue;
          }
          
        }
      }

    }
  
  }


  void Push(sNucCom Com) {

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;

      
      // Push
      nucPush NucPush = (nucPush)dlsym(Pkg.Handle, "NucPush");

      if (NucPush == NULL) {
        cout << "[ FAIL ] Pushing: " << Pkg.Package << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucPush not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucPush not found" << endl;
          continue;
        }

      } else { 
        try {
          NucPush(Com);
        }
        catch(exception &e) {
          cout << "[ FAIL ] Pushing: " << Pkg.Package << endl;

          if (Pkg.Essential)
            throw runtime_error("NucPush not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucPush not worked: " << e.what() << endl;
            continue;
          }
          
        }
      }

    }

  }

  void Pop() {

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;

      
      // Pop
      nucPop NucPop = (nucPop)dlsym(Pkg.Handle, "NucPop");

      if (NucPop == NULL) {
        cout << "[ FAIL ] Poping: " << Pkg.Package << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucPop not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucPop not found" << endl;
          continue;
        }

      } else { 
        try {
          NucPop();
        }
        catch(exception &e) {
          cout << "[ FAIL ] Poping: " << Pkg.Package << endl;

          if (Pkg.Essential)
            throw runtime_error("NucPop not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucPop not worked: " << e.what() << endl;
            continue;
          }
          
        }
      }

    }

  }


  void Load() {

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;

      
      // Load
      nucLoad NucLoad = (nucLoad)dlsym(Pkg.Handle, "NucLoad");

      if (NucLoad == NULL) {
        cout << "[ FAIL ] Loading: " << Pkg.Package << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucLoad not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucLoad not found" << endl;
          continue;
        }

      } else { 
        try {
          NucLoad();
        }
        catch(exception &e) {
          cout << "[ FAIL ] Loading: " << Pkg.Package << endl;

          if (Pkg.Essential)
            throw runtime_error("NucLoad not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucLoad not worked: " << e.what() << endl;
            continue;
          }
          
        }
      }

    }

  }

  void Unload() {

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;

      
      // Unload
      nucUnload NucUnload = (nucUnload)dlsym(Pkg.Handle, "NucUnload");

      if (NucUnload == NULL) {
        cout << "[ FAIL ] Unloading: " << Pkg.Package << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucUnload not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucUnload not found" << endl;
          continue;
        }

      } else { 
        try {
          NucUnload();
        }
        catch(exception &e) {
          cout << "[ FAIL ] Unloading: " << Pkg.Package << endl;

          if (Pkg.Essential)
            throw runtime_error("NucUnload not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucUnload not worked: " << e.what() << endl;
            continue;
          }
          
        }
      }

    }

  }


  void Start() {

    #ifdef BootAnim_Active
    #ifdef BootAnim_Progress
    int16u i = 0;
    #endif
    #endif

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;


      // Screen
      #ifdef BootAnim_Active
      #ifdef BootAnim_Progress
      BootAnim::BootAnim_Prog(Nucleols.size(), i);
      i++;
      #endif
      #endif

      
      // Start
      cout << "[ PEND ] Starting: " << Pkg.Package << endl;

      nucStart NucStart = (nucStart)dlsym(Pkg.Handle, "NucStart");

      if (NucStart == NULL) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucStart not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucStart not found" << endl;
          continue;
        }

      } else { 
        try {
          NucStart();
        }
        catch(exception &e) {
          term::Up();
          cout << "[ FAIL ]" << endl;

          if (Pkg.Essential)
            throw runtime_error("NucStart not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucStart not worked: " << e.what() << endl;
            continue;
          }
        }

      }

      term::Up();
      cout << "[  OK  ]" << endl;
    }

  }

  void Stop() {

    for (auto &Pkg: Nucleols) {

      // Skip
      if (Pkg.Handle == NULL)
        continue;

      
      // Stop
      cout << "[ PEND ] Stoping: " << Pkg.Package << endl;

      nucStop NucStop = (nucStop)dlsym(Pkg.Handle, "NucStop");

      if (NucStop == NULL) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        if (Pkg.Essential)
          throw runtime_error("NucStop not found");
        else {
          cerr << "@Amethyst: The package is not essential, it is ignored, NucStop not found" << endl;
          continue;
        }

      } else { 
        try {
          NucStop();
        }
        catch(exception &e) {
          term::Up();
          cout << "[ FAIL ]" << endl;

          if (Pkg.Essential)
            throw runtime_error("NucStop not worked: "+string(e.what()));
          else {
            cerr << "@Amethyst: The package is not essential, it is ignored, NucStop not worked: " << e.what() << endl;
            continue;
          }
        }

      }

      term::Up();
      cout << "[  OK  ]" << endl;
    }

  }

};
