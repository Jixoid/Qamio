#include <iostream>
#include <mutex>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "Widget.hpp"

#include "Standard.hpp"

using namespace std;
using namespace jix;



#pragma region __wPaint

// Basic
__wPaint::__wPaint()
{
  unique_lock Lock(Locker);

  __New();
}

__wPaint::~__wPaint()
{
  unique_lock Lock(Locker);

  __Dis();
}
  
#pragma endregion



#pragma region __wPanel

// Basic
__wPanel::__wPanel()
{
  unique_lock Lock(Locker);

  __New();
}

__wPanel::~__wPanel()
{
  unique_lock Lock(Locker);

  __Dis();
}



// Other
void __wPanel::Draw()
{
  unique_lock Lock(Locker);

  Graphic.Surface2.RectF(Surface, {
      .L = 0, .T = 0,
      .W = fSize.W,
      .H = fSize.H,
    },
    color(0.1, 0.1, 0.1, 1)
  );

}

#pragma endregion



#pragma region __wButton

// Basic
__wButton::__wButton()
{
  unique_lock Lock(Locker);

  __New();
}

__wButton::~__wButton()
{
  unique_lock Lock(Locker);

  __Dis();
}



// Other
void __wButton::Draw()
{
  unique_lock Lock(Locker);

  // Back
  Graphic.Surface2.RectF_R(Surface, {
      .L = 0, .T = 0,
      .W = fSize.W,
      .H = fSize.H,
    },
    color(0.2, 0.2, 0.2, 1),
    20
  );

  if (fFocus)
    Graphic.Surface2.RectF_R(Surface, {
        .L = 0, .T = 0,
        .W = fSize.W,
        .H = fSize.H,
      },
      color(0.47, 0.11, 0.67, 0.6),
      20
    );


  // Text
  size2d Size = Graphic.Surface2.TextSize(Surface, fText.c_str(), 20);
  
  Graphic.Surface2.Text(Surface, fText.c_str(), 20, {
    .X = (fSize.W -Size.W)/2, 
    .Y = (fSize.H -Size.H)/2,
  }, color(1,1,1,1));
  
}

#pragma endregion



#pragma region __wLabel

// Basic
__wLabel::__wLabel()
{
  unique_lock Lock(Locker);

  __New();
}

__wLabel::~__wLabel()
{
  unique_lock Lock(Locker);

  __Dis();
}



// Other
void __wLabel::Draw()
{
  unique_lock Lock(Locker);


  // Text
  size2d Size = Graphic.Surface2.TextSize(Surface, fText.c_str(), 40);
  
  Graphic.Surface2.Text(Surface, fText.c_str(), 40, {
    .X = (fSize.W -Size.W)/2, 
    .Y = (fSize.H -Size.H)/2,
  }, color(1,1,1,1));
  
}

#pragma endregion

