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
	Dev/Main.pas

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.jconf.dev/Inc \
	$(QDK)/lib.qaos.neon.dev/Inc


Links = \


Incs = $(foreach dir,$(Dirs),-Fu$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))


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


$(Bin)/Main.elf: $(Srcs)
	@echo "  - ⚙️ Main.elf"
	@$(PP) $(Incs) $(Srcs) \
		-FU$(Tmp) \
		-o$(Bin)/Main.elf
