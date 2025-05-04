include ../../Make.inc


# Config

Module = com.qaos.amethyst

Srcs = \
	Dev/Amethyst.cpp \
	Modules/Modules.cpp

Dirs = \
	$(SYS)/com.qaos.basis-devel/Inc \
	$(SYS)/com.qaos.nucleol-devel/Inc \
	$(SYS)/com.qaos.jconf-devel/Inc

Links = \
	$(SYS)/com.qaos.nucleol/$(Lib)/Nucleol.o \
	-L$(SYS)/com.qaos.jconf/$(Lib) -lJConf -Wl,-rpath,/System/Moq/com.qaos.jconf/Lib


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
	@echo "- 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Bin



$(Bin)/Main.elf: $(Bin)/Main.elf.bc
	@echo "- 🔨 Main.elf"
	@$(CP) $(Links) $(Bin)/Main.elf.bc \
		-o $(Bin)/Main.elf


$(Bin)/Main.elf.bc: $(Objs)
	@echo "- 🔗 Main.elf.bc"
	@llvm-link $(Objs) \
		-o $(Bin)/Main.elf.bc


$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "- ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

