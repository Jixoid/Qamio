include ../../Make.inc


# Info

Module = com.jixoid.graphic

Srcs = \
	Dev/Main.cpp

Dirs = \
	$(SYS)/com.qaos.basis-devel/Inc \
	$(SYS)/com.qaos.nucleol-devel/Inc \
	$(SYS)/com.qaos.jconf-devel/Inc

Links = \
	$(SYS)/com.qaos.nucleol/$(Lib)/Nucleol.o \
	-L$(SYS)/com.qaos.jconf/$(Lib)   -lJConf  -Wl,-rpath,/System/Moq/com.qaos.jconf/Lib \
  -L$(PRO)/com.jixoid.cairo/$(Lib) -lcairo  -Wl,-rpath,/Product/Moq/com.jixoid.cairo/Lib


Incs = $(foreach dir,$(Dirs),-I$(dir))
Incs += -I/usr/include/drm \

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
	@echo "- 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Nuc



$(Nuc)/Main.qo: $(Nuc)/Main.qo.bc
	@echo "- 🔨 Main.qo"
	@$(CP) -shared $(Links) $(Nuc)/Main.qo.bc \
		-o $(Nuc)/Main.qo



$(Nuc)/Main.qo.bc: $(Objs)
	@echo "- 🔗 Main.qo.bc"
	@llvm-link $(Objs) \
		-o $(Nuc)/Main.qo.bc



$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "- ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

