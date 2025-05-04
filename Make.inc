# Colors

GREEN = \e[1;32m
BLUE = \e[1;34m
ORANGE = \e[1;33m
RED = \e[1;91m
RESET = \e[0m


# Define

# $1 Mode  $2 Module  $3 Part
define Sub
	@echo -e "🧩 $(BLUE)$(3)$(RESET)::$(ORANGE)$(2)$(RESET)"
	@$(MAKE) -C $(2) $(1)
endef


# $1 Module
define Moq_Install
	@echo -e "- ⚡ Installing"
	@mkdir $(Out)/Moq/$(1)
	@tar -xf $(1)/$(Out)/Package.moq  -C $(Out)/Moq/$(1)
endef


# Params

Pre = Proj

Out = @Out
Tmp = $(Out)/Tmp
Bin = $(Out)/Bin
Lib = $(Out)/Lib
Nuc = $(Out)/Nuc


SYS = /home/alforce/Projeler/QAOS/_System
VND = /home/alforce/Projeler/QAOS/_Vendor
PRO = /home/alforce/Projeler/QAOS/_Product
KER = /home/alforce/Projeler/QAOS/_Kernel
DAT = /home/alforce/Projeler/QAOS/_Data


# Compile

ifeq ($(CONFIG_Optimize_Level_0), y)
  _O_Level = -O0
else ifeq ($(CONFIG_Optimize_Level_1), y)
  _O_Level = -O1
else ifeq ($(CONFIG_Optimize_Level_2), y)
  _O_Level = -O2
else ifeq ($(CONFIG_Optimize_Level_3), y)
  _O_Level = -O3
else ifeq ($(CONFIG_Optimize_Level_4), y)
  _O_Level = -O4
endif


CArg = -fPIC -IInc \
	-Wl,-dynamic-linker,/Lib/ld-qamio.elf \
	-Wl,-rpath,/Lib \
	\
	-Wno-unused-result \
  -Wno-unused-command-line-argument \


LC = clang   -emit-llvm -c $(_O_Level) $(CArg) $(CONFIG_CC_Params) $(CONFIG) -std=c20
LP = clang++ -emit-llvm -c $(_O_Level) $(CArg) $(CONFIG_CC_Params) $(CONFIG) -std=c++20

CC = clang   $(_O_Level) $(CArg)
CP = clang++ $(_O_Level) $(CArg)

