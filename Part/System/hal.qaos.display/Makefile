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
	Dev/Main.cpp

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/hal.qaos.display.dev/Inc

Links = \


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .bc, $(Objs))



# Targets

Build: Basis $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Nuc)


Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: $(Nuc)/Main.qo
	@echo "  - 📦️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C . Package.conf  -C $(Out) Nuc



$(Nuc)/Main.qo: $(Nuc)/Main.qo.bc
	@echo "  - 🔨 Main.qo"
	@$(CP) -shared $(Links) $(Nuc)/Main.qo.bc \
		-o $(Nuc)/Main.qo



$(Nuc)/Main.qo.bc: $(Objs)
	@echo "  - 🔗 Main.qo.bc"
	@llvm-link $(Objs) \
		-o $(Nuc)/Main.qo.bc



$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

