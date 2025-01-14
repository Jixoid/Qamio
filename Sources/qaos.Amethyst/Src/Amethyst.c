#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "JBasic.h"
#include "Nucleol.h"
#include "Wedling.h"


const char* ModulesPath = "/home/alforce/#Qamio/Mount/System/Conf/Modules.conf";


int main() {
  printf("Hi Jixoid!\n");
  printf("v%d.%d:%d-%s\n", MainVer, SubVer, PatchVer, NameVer);

  return 0;
}
