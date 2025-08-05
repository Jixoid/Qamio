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
	Dev/Kernel.cpp \
	Modules/Modules.cpp

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/lib.qaos.jconf.dev/Inc \
	$(QDK)/lib.qaos.neon.dev/Inc \
	$(QDK)/com.qaos.login.dev/Inc

Links = \
	-L$(SYS)/lib.qaos.jconf/$(Lib) -lJConf -Wl,-rpath,/System/Moq/lib.qaos.jconf/Lib \
	-L$(SYS)/lib.qaos.neon/$(Lib)  -lNeon  -Wl,-rpath,/System/Moq/lib.qaos.neon/Lib


Incs = $(foreach dir,$(Dirs),-I$(dir))
Incs+= -IModules

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .bc, $(Objs))




# Targets

Build: Basis $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Bin)
	

Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: $(Bin)/Main.elf
	@echo "  - 📦️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C . Package.conf  -C $(Out) Bin



$(Bin)/Main.elf: $(Bin)/Main.elf.bc
	@echo "  - 🔨 Main.elf"
	@$(CP) $(Links) $(Bin)/Main.elf.bc \
		-o $(Bin)/Main.elf


$(Bin)/Main.elf.bc: $(Objs)
	@echo "  - 🔗 Main.elf.bc"
	@llvm-link $(Objs) \
		-o $(Bin)/Main.elf.bc


$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

