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
	Dev/JConf.cc \

Dirs = \
	$(QDK)/lib.qaos.basis.dev/Inc \
	$(QDK)/lib.qaos.kernel.dev/Inc \
	$(QDK)/lib.qaos.jconf.dev/Inc


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))




# Targets

Build: Basis $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "Base.moqs $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Base.moqs
	@tar -cf $(Out)/Base.moqs  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Lib)/libJConf.a $(Lib)/libJConf.so
	@echo -e $(L_MQ) "Base.moq"
	@$(MQ) Package.conf $(Lib) \
		$(MQ2)



$(Lib)/libJConf.so: $(Objs)
	@echo -e $(L_LD) "libJConf.so $(GRAY)($(notdir $^))$(RESET)"
	@$(CP) -shared -o $(Lib)/libJConf.so \
		$(Objs) $(Links)


$(Lib)/libJConf.a: $(Objs)
	@echo -e $(L_AR) "libJConf.a $(GRAY)($(notdir $^))$(RESET)"
	@$(AR) rcs $(Lib)/libJConf.a \
		$(Objs)



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CP) "$(notdir $@) $(GRAY)($(notdir $<))$(RESET)"
	@$(CP) -c -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

