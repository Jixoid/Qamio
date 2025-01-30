#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

#include "Nucleol.hpp"
#include "Screen.hpp"
#include "Graphic.hpp"
#include "Widget.hpp"

extern "C" {
  #include <string.h>
}

using namespace std;
using namespace jix;


#define ComSet(X) NucCom.Set(#X, (point)X);




#pragma region Publish

vector<int32u> Gpus;
struct sScr {
  int32u Gpu;
  int32u Scr;
  sScreen Screen;
};
vector<sScr> Scrs;


thread* RoutineT = NULL;
atomic<bool> RoutineR;


class form1 {

  public:
    wpanel  Panel;
    wbutton Button;

} Form1;


void  RoutineM() {
  cout << "Thread Started" << endl;
  
  Form1.Panel = Widget::Widget_Panel_New(Nil);
  Widget::Widget_Visual_SizeSet(Form1.Panel, {
    .Width  = int32f(Scrs[0].Screen.Mode.Width),
    .Height = int32f(Scrs[0].Screen.Mode.Height),
  });


  Form1.Button = Widget::Widget_Button_New(Form1.Panel);
  Widget::Widget_Visual_PointSet(Form1.Button, {
    .X = 20, .Y = 20,
  });

  while(RoutineR.load()) {
    surface2 Sur = Graphic::Window2_Sur(Scrs[0].Screen.Window);

    // Draw
    Widget::Widget_Visual_Draw(Form1.Panel);
    surface2 Wid = Widget::Widget_Visual_Buf(Form1.Panel);


    // Connect
    Graphic::Surface2_Draw_Sur2(Sur, Wid, {0,0});


    // Swap
    Graphic::Window2_Swp(Scrs[0].Screen.Window);
    Screen::Screen_ScrSwap(Scrs[0].Gpu, Scrs[0].Scr);

    this_thread::sleep_for(chrono::milliseconds(2));
  }

  Widget::Widget_Widget_Dis(Form1.Button);
  Widget::Widget_Widget_Dis(Form1.Panel);

  cout << "Thread Stoped" << endl;
}


void  Login_ScrStart() {
  
  #pragma region Prepare

  // Gpu
  Screen::Screen_GpuFind();
  int32u GpuC = Screen::Screen_GpuCount();
  
  if (0 == GpuC) {
    cout << "@Login: Not found a Gpu" << endl;
    return;
  }

  for (int32u i = 0; i < GpuC; i++) {
    Screen::Screen_GpuOpen(i);
    Gpus.push_back(i);
  }


  // Scr
  for (int32u i = 0; i < GpuC; i++) {
    Screen::Screen_ScrFind(i);
    int32u ScrC = Screen::Screen_ScrCount(i);
    
    for (int32u j = 0; j < ScrC; j++) {
      Screen::Screen_ScrOpen(i,j);
      auto Scr = Screen::Screen_ScrGet(i,j);

      Scrs.push_back({
        .Gpu = i,
        .Scr = j,
        .Screen = Scr
      });

      //cout << "Screen[" << i << "," << j << "]: " << Scr.Mode.Width << "x" << Scr.Mode.Height << "@" << Scr.Mode.Refresh << endl;
    }
  }

  #pragma endregion


  if (RoutineT != NULL)
    throw runtime_error("@Login: Thread not started");

  cout << "Thread Starting" << endl;

  RoutineR.store(true);
  RoutineT = new thread(RoutineM);
}

void  Login_ScrStop() {

  if (RoutineT == NULL)
    throw runtime_error("@Login: Thread not stoped");

  cout << "Thread Stoping" << endl;

  RoutineR.store(false);
  RoutineT->join();

  delete RoutineT; RoutineT = NULL;


  #pragma region Prepare

  // Free Screen
  for (auto &Scr: Scrs) {
    Screen::Screen_ScrClose(Scr.Gpu, Scr.Scr);
  }
  Scrs.clear();


  // Free Gpu
  for (auto &Gpu: Gpus) {
    Screen::Screen_GpuClose(Gpu);
  }
  Gpus.clear();

  #pragma endregion

}

#pragma endregion



// Global
sNucCom NucCom;


#pragma region NucCom
extern "C" bool NucCheck(int16u Ver) {
  return (gnucVer == Ver);
}


extern "C" void NucPush(sNucCom Com) {

  NucCom = Com;

  ComSet(Login_ScrStart);
  ComSet(Login_ScrStop);
}

extern "C" void NucPop() {

  Screen::PopNuc(NucCom);
  Graphic::PopNuc(NucCom);
  Widget::PopNuc(NucCom);
}


extern "C" void NucLoad() {

}

extern "C" void NucUnload() {

}


extern "C" void NucStart() {
  
}

extern "C" void NucStop() {
  
}

#pragma endregion
