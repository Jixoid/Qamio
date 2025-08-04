#
#  This file is part of QAOS.
#
#  This file is licensed under the GNU Lesser General Public License version 3 (LGPLv3).
#
#  You should have received a copy of GNU Lesser General Public License
#  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
#
#  Copyright (c) 2025 by Kadir Aydın.
#



# Colors

GRAY = \e[1;30m
GREEN = \e[1;32m
BLUE = \e[1;34m
ORANGE = \e[1;33m
RED = \e[1;91m
RESET = \e[0m


# Params

Out = @Out
Tmp = $(Out)/Tmp
Bin = $(Out)/Bin
Lib = $(Out)/Lib
Nuc = $(Out)/Nuc


Locate = /home/alforce/Projeler/QAOS
QDK = $(Locate)/QDK
SYS = $(Locate)/Part/System
VND = $(Locate)/Part/Vendor
PRO = $(Locate)/Part/Product
DAT = $(Locate)/Part/Data


# Compile

ifeq ($(CONFIG_Optimize_Level_0), y)
  _O_Level = -O0
else ifeq ($(CONFIG_Optimize_Level_1), y)
  _O_Level = -O1
else ifeq ($(CONFIG_Optimize_Level_2), y)
  _O_Level = -O2
else ifeq ($(CONFIG_Optimize_Level_3), y)
  _O_Level = -O3
endif


CArg = -fPIC -IInc \
	-Wl,-dynamic-linker,/System/Moq/lib.qaos.bootstrap/Lib/ld-qamio.elf \
	-Wl,-rpath,/System/Moq/lib.qaos.bootstrap/Lib \
  -rdynamic \
	\
	-Wno-unused-result \
  -Wno-unused-command-line-argument \


LC = clang   -emit-llvm -c $(_O_Level) $(CArg) $(CONFIG_CC_Params) -include $(Locate)/config.h -std=c23
LP = clang++ -emit-llvm -c $(_O_Level) $(CArg) $(CONFIG_CC_Params) -include $(Locate)/config.h -std=c++20

CC = clang   $(_O_Level) $(CArg)
CP = clang++ $(_O_Level) $(CArg)

