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
	Dev/JConf.cpp \

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/lib.qaos.jconf.dev/Inc


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .bc, $(Objs))




# Targets

Build: Basis $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: Package.conf $(Lib)/libJConf.so
	@echo "  - 📦️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C . Package.conf  -C $(Out) Lib



$(Lib)/libJConf.so: $(Lib)/libJConf.so.bc
	@echo "  - 🔨 libJConf.so"
	@$(CP) -shared $(Lib)/libJConf.so.bc \
		-o $(Lib)/libJConf.so


$(Lib)/libJConf.so.bc: $(Objs)
	@echo "  - 🔗 libJConf.so.bc"
	@llvm-link $(Objs) \
		-o $(Lib)/libJConf.so.bc


$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

