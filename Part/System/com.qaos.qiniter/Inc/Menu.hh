/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>
#include <filesystem>
#include <vector>

#include <termios.h>
#include <unistd.h>

#include "Basis.h"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;



extern struct winsize w;
extern struct termios oldt, newt;


void disableRawMode();
void enableRawMode();


void DrawMenu(string Caption, vector<string> List, u32 Index);

string Menu(string Caption, vector<string> List);

