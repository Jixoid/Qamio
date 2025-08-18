/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>

#include "Basis.h"


using namespace std;
using namespace jix;


int get_free_loop_device();

void create_loop_device(string path, int loop_num);

void setup_loop_device(const string& imagePath, const string& loopDevice);

