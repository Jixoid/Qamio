#include <iostream>
#include <fstream>
#include <vector>

#include "JBasic.hpp"
#include "Error.hpp"
#include "Terminal.cpp"

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
      point Handle = (point)dlopen(("/Pkg/"+Pkg.Package+"/Lib/Main.so").c_str(), RTLD_NOW);
      Pkg.Handle = Handle;

      if (!Handle && Pkg.Essential) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        throw runtime_error( dlerror() );
      }

      term::Up();
      cout << "[  OK  ]" << endl;
    }

  }

  void Check() {

    for (auto &Pkg: Nucleols) {
      
      // Check
      nucCheck NucCheck = (nucCheck)dlsym(Pkg.Handle, "NucCheck");
      if (!NucCheck && Pkg.Essential) {
        cout << "[ FAIL ] Checking: " << Pkg.Package << endl;
        
        throw runtime_error("NucCheck not found");
      }

      try {
        if (!NucCheck(gnucVer))
          throw runtime_error(Pkg.Package+" is not compatible");
      }
      catch(runtime_error e){
        cout << "[ FAIL ] Checking: " << Pkg.Package << endl;
        throw e;
      }
      catch(exception e) {
        cout << "[ FAIL ] Checking: " << Pkg.Package << endl;
        
        throw runtime_error("NucCheck not worked");
      }

    }

  }

  void Load() {

    for (auto &Pkg: Nucleols) {
      
      // Load
      nucLoad NucLoad = (nucLoad)dlsym(Pkg.Handle, "NucLoad");
      if (!NucLoad && Pkg.Essential) {
        cout << "[ FAIL ] Loading: " << Pkg.Package << endl;
        
        throw runtime_error("NucLoad not found");
      }

      try {
        NucLoad();
      }
      catch(exception e) {
        cout << "[ FAIL ] Loading: " << Pkg.Package << endl;
        
        throw runtime_error("NucLoad not worked");
      }

    }

  }

  void Start() {

    for (auto &Pkg: Nucleols) {

      cout << "[ PEND ] Starting: " << Pkg.Package << endl;
      
      // Start
      nucStart NucStart = (nucStart)dlsym(Pkg.Handle, "NucStart");
      if (!NucStart && Pkg.Essential) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        throw runtime_error("NucStart not found");
      }

      try {
        NucStart();
      }
      catch(exception e) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        throw runtime_error("NucStart not worked");
      }

      term::Up();
      cout << "[  OK  ]" << endl;
    }

  }

  void Stop() {

    for (auto &Pkg: Nucleols) {

      cout << "[ PEND ] Stoping: " << Pkg.Package << endl;
      
      // Stop
      nucStop NucStop = (nucStop)dlsym(Pkg.Handle, "NucStop");
      if (!NucStop && Pkg.Essential) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        throw runtime_error("NucStop not found");
      }

      try {
        NucStop();
      }
      catch(exception e) {
        term::Up();
        cout << "[ FAIL ]" << endl;
        
        throw runtime_error("NucStop not worked");
      }

      term::Up();
      cout << "[  OK  ]" << endl;
    }

  }

  void Unload() {

    for (auto &Pkg: Nucleols) {
      
      // Unload
      nucUnload NucUnload = (nucUnload)dlsym(Pkg.Handle, "NucUnload");
      if (!NucUnload && Pkg.Essential) {
        cout << "[ FAIL ] Unloading: " << Pkg.Package << endl;
        
        throw runtime_error("NucUnload not found");
      }

      try {
        NucUnload();
      }
      catch(exception e) {
        cout << "[ FAIL ] Unloading: " << Pkg.Package << endl;
        
        throw runtime_error("NucUnload not worked");
      }

    }

  }


};
