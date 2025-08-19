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
	Dev/Main.cc

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/lib.qaos.neon.dev/Inc \
	$(QDK)/com.qaos.sysui.dev/Inc \
	$(QDK)/com.qaos.user.dev/Inc \
	$(QDK)/hal.qaos.display.dev/Inc \
	$(QDK)/hal.qaos.graphic.dev/Inc \
	$(QDK)/com.qaos.composer.display.dev/Inc

Links = \
	-L$(SYS)/lib.qaos.neon/$(Lib)  -lNeon  -Wl,-rpath,/System/Moq/lib.qaos.neon/Lib	


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))



# Targets

Build: Basis $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Nuc)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "Base.moqs $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Base.moqs
	@tar -cf $(Out)/Base.moqs  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Nuc)/Main.qo
	@echo -e $(L_MQ) "Base.moq $(GRAY)($(notdir $^))$(RESET)"
	@$(MQ) Package.conf $(Nuc) \
		$(MQ2)



$(Nuc)/Main.qo: $(Objs)
	@echo -e $(L_LD) "Main.qo $(GRAY)($(notdir $^))$(RESET)"
	@$(CP) -shared -o $(Nuc)/Main.qo \
		$(Objs) $(Links)



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CP) "$(notdir $@) $(GRAY)($(notdir $<))$(RESET)"
	@$(CP) -c -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

