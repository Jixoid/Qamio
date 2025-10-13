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

Srcs = \
	Dev/Kernel.cc \
	Modules/Modules.cc

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/lib.qaos.jconf.dev/Inc \
	$(QDK)/lib.qaos.neon.dev/Inc \
	$(QDK)/com.qaos.login.dev/Inc

Links = \
	-Wl,-rpath,/System/Moq/lib.qaos.bootstrap/Lib \
	-L$(SYS)/lib.qaos.jconf/$(Lib) -lJConf -Wl,-rpath,/System/Moq/lib.qaos.jconf/Lib \
	-L$(SYS)/lib.qaos.neon/$(Lib)  -lNeon  -Wl,-rpath,/System/Moq/lib.qaos.neon/Lib


Incs = $(foreach dir,$(Dirs),-I$(dir))
Incs+= -IModules

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))




# Targets

Build: Basis $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Bin)
	

Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "$(notdir $@) $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $@
	@tar -cf $@  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Bin)/Main.elf
	@echo -e $(L_MQ) "$(notdir $@) $(GRAY)($(notdir $^))$(RESET)"
	@$(MQ) Package.conf $(Bin) \
		$(MQ2)



$(Bin)/Main.elf: $(Objs)
	@echo -e $(L_LD) "$(notdir $@) $(GRAY)($(notdir $^))$(RESET)"
	@$(CX) -o $@ \
		$(Objs) $(Links)



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CX) "$(notdir $@) $(GRAY)($(notdir $<))$(RESET)"
	@$(CX) -c -DModule=\"$(Module)\" $(Incs) $< -o $@

