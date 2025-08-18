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
	$(QDK)/hal.qaos.display.dev/Inc \
	$(QDK)/hal.qaos.graphic.dev/Inc \
	$(QDK)/com.qaos.composer.display.dev/Inc

Links = \


Incs = $(foreach dir,$(Dirs),-I$(dir))
Incs+= -IModules

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))




# Targets

Build: Basis $(Out)/Package.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Nuc)
	

Clean:
	@rm -rf $(Out)



# Files
$(Out)/Package.moqs: $(Out)/Package.moq
	@echo -e $(L_SG) "Package.moqs"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Package.moqs
	@tar -cf $(Out)/Package.moqs  -C $(Out) Package.moq Package.moq.sig


$(Out)/Package.moq: Package.conf $(Nuc)/Main.qo
	@echo -e $(L_MQ) "Package.moq"
	@$(MQ) Package.conf $(Nuc) \
		$(MQ2)



$(Nuc)/Main.qo: $(Objs)
	@echo -e $(L_LD) "Main.qo"
	@$(CP) -shared -o $(Nuc)/Main.qo \
		$(Objs) $(Links)



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CP) "$(notdir $@)"
	@$(CP) -c -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

