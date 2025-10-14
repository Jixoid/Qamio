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


# Bash
SHELL = /bin/bash


# Colors
GRAY = \e[1;30m
GREEN = \e[1;32m
BLUE = \e[1;34m
ORANGE = \e[1;33m
RED = \e[1;91m
RESET = \e[0m


# Locates
Locate = /home/alforce/Projeler/QAOS
QDK = $(Locate)/QDK
EXT = $(Locate)/ExternLib
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


ifeq ($(CONFIG_Arch_X64), y)
  Arch = X64
  CTarget = x86_64
  PTarget = x86_64
else ifeq ($(CONFIG_Arch_X32), y)
  Arch = X32
  CTarget = i386
  PTarget = i386

else ifeq ($(CONFIG_Arch_A64), y)
  Arch = A64
  CTarget = aarch64
  PTarget = aarch64
else ifeq ($(CONFIG_Arch_A32), y)
  Arch = A32
  CTarget = arm
  PTarget = arm

else ifeq ($(CONFIG_Arch_R64), y)
  Arch = R64
  CTarget = riscv64
  PTarget = riscv64
else ifeq ($(CONFIG_Arch_R32), y)
  Arch = R32
  CTarget = riscv32
  PTarget = riscv32

else ifeq ($(CONFIG_Arch_P64), y)
  Arch = P64
  CTarget = powerpc64le
  PTarget = powerpc64le
endif


ifeq ($(CONFIG_Debug), y)
  Debug = "-g"
else
  Debug = ""
endif


ifeq ($(CONFIG_Sanitizer), y)
  Sanitizer = "-fsanitize=address,leak,undefined,bounds"
else
  Sanitizer = ""
endif



# Params
GOut = !Out
Out = $(GOut)/$(Arch)
Tmp = $(Out)/Tmp
Bin = $(Out)/Bin
Lib = $(Out)/Lib
Nuc = $(Out)/Nuc
Mod = $(Out)/Mod


#--target=$(CTarget) -I/usr/include/c++/15 -I/usr/include/c++/15/$(CTarget)-redhat-linux/
CArg = -fPIC -IInc $(_O_Level) $(Debug) \
  \
  $(addprefix -D,$(shell cat $(Locate)/def.list)) \
  \
	-Wl,-dynamic-linker,/System/Moq/lib.qaos.bootstrap/Lib/ld-qamio.elf \
	-Wl,-rpath,/System/Moq/lib.qaos.bootstrap/Lib \
  -rdynamic \
  \
  -Wno-unused-command-line-argument



CC = clang    $(CArg) $(CONFIG_CC_Params) -std=c23
CX = clang++  $(CArg) $(Sanitizer) $(CONFIG_CP_Params) -std=c++23
CX_NDBG = clang++  $(CArg) $(CONFIG_CP_Params) -std=c++23

AR = $(CTarget)-linux-gnu-ar $(CONFIG_AR_Params)
LD = $(CTarget)-linux-gnu-ld $(CONFIG_LD_Params)

MQ  = mksquashfs
MQ2 = $(Out)/Base.moq -quiet -comp xz -noappend; echo -ne "\033[A\033[2K"

SHA = openssl dgst -sha512 -binary $(Out)/Base.moq > $(Out)/Base.moq.sha512
SIG = openssl pkeyutl -sign -in $(Out)/Base.moq.sha512 -inkey $(Locate)/!Dev/Key_priv.pem -out $(Out)/Base.sig


L_EXT = "    📤 $(GRAY)EXT$(RESET)"
L_CFG = "    🔧 $(GRAY)CFG$(RESET)"
L_DWL = "    ⬇️ $(GRAY)DWL$(RESET)"
L_BLD = "    🛠️ $(GRAY)BLD$(RESET)"

L_CC = "    ⚙️ $(GRAY)CC$(RESET)"
L_CX = "    ⚙️ $(GRAY)CX$(RESET)"

L_AR = "    🔗 $(GRAY)AR$(RESET)"
L_LD = "    🔗 $(GRAY)LD$(RESET)"

L_MQ = "    📦️ $(GRAY)MQ$(RESET)"
L_SG = "    ✒️ $(GRAY)SG$(RESET)"
