/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define el else
#define ef else if

#include <iostream>
#include <filesystem>
#include <vector>

#include <termio.h>
#include <unistd.h>

#include "Basis.h"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;



struct winsize w;
struct termios oldt, newt;


void disableRawMode()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void enableRawMode()
{
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO); // Canonical mode kapalı, echo kapalı
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  atexit(disableRawMode);
}


void DrawMenu(string Caption, vector<string> List, u32 Index)
{
  printf("\033[2J\033[H");
  cout << "\033[1;34m""QIniter""\033[1;30m"": ""\033[0m"+Caption+"\033[0m" << endl << endl;


  for (u32 i = 0; i < w.ws_col; i++)
    cout << "_";
  cout << endl << endl;

  for (u32 i = 0; i < List.size(); i++)
    cout << ((i == Index) ? "\033[7m""> ":"  ") << List[i] << ((i == Index) ? "\033[27m":"") << endl;

  for (u32 i = 0; i < w.ws_col; i++)
    cout << "_";
  cout << endl;
}

string Menu(string Caption, vector<string> List)
{
  string Ret;
  i32 Index = 0;

  while (Ret.empty())
  {
    DrawMenu(Caption, List, Index);

    char c = getchar();

    if (c == '\033')
    { // ESC ise
      getchar();     // '[' karakterini oku
      switch (getchar())
      {
        case 'A': // Yukarı
          Index -= 1;
          if (Index < 0)
            Index = 0;

          break;

        case 'B': // Aşağı
          Index += 1;
          if (Index > List.size()-1)
            Index = List.size()-1;

          break;
      }
    }
    
    ef (c == '\n')
    {
      Ret = List[Index];
      break;
    }

  }

  return "";
}

