#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)





void Painter(wVisual Sender, surface2 Surface)
{
  size2d Size = Graphic.Surface2.SizeGet(Surface);

  Graphic.Surface2.RectF(Surface, {0,0, Size.W,Size.H}, color(0.2, 0.2, 0.2));
}



#pragma region Publish

wVisual _New()
{
  // Interface
  wVisual Ret = Widget.WVisual.New();

  Widget.WVisual.mPaintSet(Ret, &Painter);
  

  // Return
  return Ret;
}

#pragma endregion






// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(SystemUI_Notify)
  {
    .New = &_New,
  };
}

void Pop()
{
  PopNuc(Widget);
  PopNuc(Graphic);
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,
};

#pragma endregion
