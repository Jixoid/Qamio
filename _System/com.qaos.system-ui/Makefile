include ../../Make.inc


# Config

Module = com.qaos.system-ui

Srcs = \
	Dev/Main.cpp

Dirs = \
	$(QDK)/dev.qaos.basis/Inc \
	$(QDK)/dev.qaos.nucleol/Inc

Links = \
	$(SYS)/lib.qaos.nucleol/$(Lib)/Nucleol.o


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
	@echo "  - 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Nuc



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

