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
	Dev/Neon.cc \
	Dev/Neon_Error.cc

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/lib.qaos.neon.dev/Inc


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))




# Targets

Build: Basis $(Out)/Package.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Package.moqs: $(Out)/Package.moq
	@echo -e $(L_SG) "Package.moqs"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Package.moqs
	@tar -cf $(Out)/Package.moqs  -C $(Out) Package.moq Package.moq.sig


$(Out)/Package.moq: Package.conf $(Lib)/libNeon.so $(Lib)/libNeon.a
	@echo -e $(L_MQ) "Package.moq"
	@$(MQ) Package.conf $(Lib) \
		$(MQ2)



$(Lib)/libNeon.so: $(Objs)
	@echo -e $(L_LD) "libNeon.so"
	@$(CP) -shared -o $(Lib)/libNeon.so \
		$(Objs) $(Links)


$(Lib)/libNeon.a: $(Objs)
	@echo -e $(L_AR) "libNeon.a"
	@$(AR) rcs $(Lib)/libNeon.a \
		$(Objs)



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CP) "$(notdir $@)"
	@$(CP) -c -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

