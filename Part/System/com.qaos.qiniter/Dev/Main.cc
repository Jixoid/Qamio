/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <sys/reboot.h>
#include <sys/utsname.h>
#include <linux/reboot.h>
#include <unistd.h>
#include <wait.h>
#include <termios.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <linux/loop.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sysmacros.h> 
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include "Basis.h"

#include "Menu.hh"
#include "Loop.hh"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;



// Global
void SysCtl_Shutdown()
{
  cout << "----- Power Off -----" << endl;
  reboot(LINUX_REBOOT_CMD_POWER_OFF);

  exit(0);
}

void PANIC(string Msg)
{
  cout << endl;

  cout << "\033[1;91m""PANIC""\033[1;30m"": ""\033[0m" << Msg << endl;
  cout << endl;

  sleep(5);

  SysCtl_Shutdown();
}


inline void TermUp()
{
  cout << "\033[1A";
}



vector<unsigned char> sha512_file(const string &path)
{
  ifstream file(path, ios::binary);
  if (!file.is_open())
      PANIC("File can't opened: "+path);

  
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx)
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_MD_CTX_new failed");
    #else
      PANIC("crypto: internal error");
    #endif

  
  if (EVP_DigestInit_ex(ctx, EVP_sha512(), nullptr) != 1)
  {
    EVP_MD_CTX_free(ctx);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_DigestInit_ex failed");
    #else
      PANIC("crypto: internal error");
    #endif
  }

  array<char, 8192> buffer{};
  while (file.good())
  {
    file.read(buffer.data(), buffer.size());
    streamsize bytesRead = file.gcount();
    if (bytesRead > 0)
    {
      if (EVP_DigestUpdate(ctx, buffer.data(), static_cast<size_t>(bytesRead)) != 1)
      {
        EVP_MD_CTX_free(ctx);
        #ifdef CONFIG_QInit_MsgDebug
          PANIC("openssl: EVP_DigestUpdate failed");
        #else
          PANIC("crypto: internal error");
        #endif
      }
    }
  }

  vector<unsigned char> hash(EVP_MD_size(EVP_sha512()));
  unsigned int hashLen = 0;
  if (EVP_DigestFinal_ex(ctx, hash.data(), &hashLen) != 1)
  {
    EVP_MD_CTX_free(ctx);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_DigestFinal_ex failed");
    #else
      PANIC("crypto: internal error");
    #endif
  }
  hash.resize(hashLen);

  EVP_MD_CTX_free(ctx);
  return hash;
}

bool verify_signature(const string &dataPath, const string &sigPath, const string &pubKeyPath)
{
  // 1. Public key yükle
  EVP_PKEY *pkey = EVP_PKEY_new();
  if (pkey == Nil)
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_PKEY_new failed");
    #else
      PANIC("crypto: internal error");
    #endif

  FILE *pubf = fopen(pubKeyPath.c_str(), "r");
  if (!pubf)
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("keybox: Key \""+pubKeyPath+"\" can't opened");
    #else
      PANIC("keybox: internal error");
    #endif

  PEM_read_PUBKEY(pubf, &pkey, NULL, NULL);
  fclose(pubf);



  // 2. Verify context hazırla
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
  if (!ctx)
  {
    EVP_PKEY_free(pkey);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_PKEY_CTX_new failed");
    #else
      PANIC("crypto: internal error");
    #endif
  }

  if (EVP_PKEY_verify_init(ctx) <= 0)
  {
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_PKEY_verify_init failed");
    #else
      PANIC("crypto: internal error");
    #endif
  }

  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
  {
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("openssl: EVP_PKEY_CTX_set_rsa_padding failed");
    #else
      PANIC("crypto: internal error");
    #endif
  }



  // 3. Hash verisini oku (64 byte binary)
  vector<unsigned char> hash = sha512_file(dataPath);
  if (hash.size() != 64)
  {
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("read: file can't readed: "+dataPath);
    #else
      PANIC("crypto: internal error");
    #endif
  }

  

  // 4. İmzayı oku
  ifstream fsig(sigPath.c_str(), ios::binary);
  if (!fsig.is_open())
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("read: file can't opened: "+sigPath);
    #else
      PANIC("Sign \""+sigPath+"\" can't opened");
    #endif


  vector<unsigned char> sig((istreambuf_iterator<char>(fsig)), {});


  // 5. Verify yap
  return (EVP_PKEY_verify(ctx, sig.data(), sig.size(), hash.data(), hash.size()) == 1);
}



void ActivateModule(string Part, string Pkg)
{     
  if (!fs::is_regular_file("/"+Part+"/Pkg/"+Pkg+"/Base.moq"))
    PANIC(Pkg+" can't found");

  if (!fs::is_regular_file("/"+Part+"/Pkg/"+Pkg+"/Base.sig"))
    PANIC(Pkg+" sign can't found");

  
  // Verify Kernel
  if (!verify_signature("/"+Part+"/Pkg/"+Pkg+"/Base.moq", "/"+Part+"/Pkg/"+Pkg+"/Base.sig", "/Keybox/1.pem"))
    PANIC(Pkg+" can't be trusted");

  
  // Mount Kernel
  int KLoop = get_free_loop_device();

  if (access(("/dev/loop"+to_string(KLoop)).c_str(), F_OK) != 0)
    create_loop_device("/dev/loop"+to_string(KLoop), KLoop);


  setup_loop_device("/"+Part+"/Pkg/"+Pkg+"/Base.moq", "/dev/loop"+to_string(KLoop));


  mkdir(("/"+Part+"/Moq/"+Pkg).c_str(), 0740);

  if (mount(("/dev/loop"+to_string(KLoop)).c_str(), ("/"+Part+"/Moq/"+Pkg).c_str(), "squashfs", MS_RDONLY, nullptr) != 0)
    PANIC(Pkg+" can't activated");
}



char** build_args(const vector<string>& params)
{
  // +1, nullptr terminator için
  char** argv = new char*[params.size() + 1];
  
  for (size_t i = 0; i < params.size(); ++i) {
    // her parametre için kopya oluştur
    argv[i] = new char[params[i].size() + 1];
    strcpy(argv[i], params[i].c_str());
  }
  
  argv[params.size()] = nullptr; // sonlandırıcı
  return argv;
}

void Main()
{
  printf("\033[2J\033[H");
  cout << "\033[1;34m""QIniter""\033[0m" << endl;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    PANIC("ioctl: internal error");

  if (w.ws_col == 0) w.ws_col = 80;
  if (w.ws_row == 0) w.ws_row = 24;



  if (mount("none", "/sys",  "sysfs", 0, Nil) != 0)
    PANIC("can't mounted: sys");
  
  if (mount("none", "/proc", "proc",  0, Nil) != 0)
    PANIC("can't mounted: proc");


  
  vector<string> Systems;  string Sys;
  vector<string> Vendors;  string Ven;
  vector<string> Products; string Pro;

  for (const auto &Entry: fs::directory_iterator("/Slots/System"))
    if (Entry.is_directory())
      Systems.push_back(Entry.path().filename().string());

  for (const auto &Entry: fs::directory_iterator("/Slots/Vendor"))
    if (Entry.is_directory())
      Vendors.push_back(Entry.path().filename().string());

  for (const auto &Entry: fs::directory_iterator("/Slots/Product"))
    if (Entry.is_directory())
      Products.push_back(Entry.path().filename().string());


  if (Systems.size() == 0)
    PANIC("No ""\033[1;33m""SYSTEM""\033[0m"" found to boot from");
  if (Systems.size() == 1)
    Sys = Systems[0];

  if (Vendors.size() == 0)
    PANIC("No ""\033[1;33m""VENDOR""\033[0m"" found to boot from");
  if (Vendors.size() == 1)
    Ven = Vendors[0];

  if (Products.size() == 0)
    PANIC("No ""\033[1;33m""PRODUCT""\033[0m"" found to boot from");
  if (Products.size() == 1)
    Pro = Products[0];

  
  enableRawMode();

  if (Sys.empty())
    Sys = Menu("Select ""\033[1;33m""SYSTEM""\033[0m"" slot", Systems);

  if (Ven.empty())
    Ven = Menu("Select ""\033[1;33m""VENDOR""\033[0m"" slot", Vendors);

  if (Pro.empty())
    Pro = Menu("Select ""\033[1;33m""PRODUCT""\033[0m"" slot", Products);

  disableRawMode();
  

  printf("\033[2J\033[H");
  cout << "\033[1;34m""QIniter""\033[0m" << endl << endl;



  // Mount
  cout << "[""\033[1;33m""PEND""\033[0m""] Mounting" << endl;

  if (mount(("/Slots/System/"+Sys).c_str(), "/System", Nil, MS_BIND, Nil) != 0)
    PANIC("Can't mounted ""\033[1;33m""SYSTEM""\033[0m");

  if (mount(("/Slots/Vendor/"+Ven).c_str(), "/Vendor", Nil, MS_BIND, Nil) != 0)
    PANIC("Can't mounted ""\033[1;33m""VENDOR""\033[0m");

  if (mount(("/Slots/Product/"+Pro).c_str(), "/Product", Nil, MS_BIND, Nil) != 0)
    PANIC("Can't mounted ""\033[1;33m""PRODUCT""\033[0m");


  TermUp();
  cout << "[ ""\033[1;32m""OK""\033[0m"" ]" << endl;
  



  // Check
  cout << "[""\033[1;33m""PEND""\033[0m""] Activating" << endl;

  //ActivateModule("/System/Moq/com.qaos.kernel", "Kernel");

  for (string Part: {"System", "Vendor", "Product"})
    for (const auto &Entry: fs::directory_iterator("/"+Part+"/Pkg"))
      if (Entry.is_directory())
        ActivateModule(Part, Entry.path().filename().string());
      

  TermUp();
  cout << "[ ""\033[1;32m""OK""\033[0m"" ]" << endl;



  // Exec
  cout << "\033[2J\033[H" << flush;
  

  char** Args = build_args({
    "/System/Moq/com.qaos.kernel/Bin/Main.elf",
    "-initer=qiniter",
    "-slot-sys="+Sys,
    "-slot-ven="+Ven,
    "-slot-pro="+Pro,
  });

  char** Envs = build_args({
  
  });

  if(execve(Args[0], Args, Envs) != 0)
    PANIC("Kernel can't started");
}


int main()
{
  try {
    Main();
  } 
  catch(exception &e) {
    cerr << "An exception was encountered" << endl;
    cerr << "Error class: " << typeid(e).name() << endl;
    cerr << "Error message: " << e.what() << endl;

    sleep(1);
  }

  SysCtl_Shutdown();
}
