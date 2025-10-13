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



include ../../../Make.inc


# Config

Module = $(notdir $(CURDIR))

KDIR := $(EXT)/ext.linux/$(Out)/linux
CWD := $(shell pwd)

Srcs = \
	Dev/Main.c \
	Dev/Handler.c

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))


# Targets

Build: Basis $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Mod)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "$(notdir $@) $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $@
	@tar -cf $@  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Mod)/Main.ko
	@echo -e $(L_MQ) "$(notdir $@) $(GRAY)($(notdir $^))$(RESET)"
	@$(MQ) Package.conf $(Mod) \
		$(MQ2)


obj-m += $(Tmp)/Main.o

$(Mod)/Main.ko: $(Srcs)
	@echo -e $(L_CC) "$(notdir $@) $(GRAY)($(notdir $^))$(RESET)"
	@cp $(Srcs)  $(Tmp)/

	@$(MAKE) -C $(KDIR) M=$(CWD) CC=/usr/bin/gcc EXTRA_CFLAGS"-DModule=\"$(Module)\"" modules > /dev/null

	@find -iname "*module*" -exec rm {} \;

	@cp $(Tmp)/Main.ko $@

