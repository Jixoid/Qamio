include ../../Make.inc


# Config

Module = lib.qaos.nucleol

Srcs = \
	Dev/Nucleol.cpp \

Dirs = \
	$(QDK)/dev.qaos.basis/Inc \
	$(QDK)/dev.qaos.nucleol/Inc


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .bc, $(Objs))


# Targets

Build: Basis  $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: $(Lib)/Nucleol.o
	@echo "  - 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Lib



$(Lib)/Nucleol.o: $(Lib)/Nucleol.o.bc
	@echo "  - 🔨 Nucleol.o"
	@$(CP) -c $(Lib)/Nucleol.o.bc \
		-o $(Lib)/Nucleol.o


$(Lib)/Nucleol.o.bc: $(Objs)
	@echo "  - 🔗 Nucleol.o.bc"
	@llvm-link $(Objs) \
		-o $(Lib)/Nucleol.o.bc


$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

