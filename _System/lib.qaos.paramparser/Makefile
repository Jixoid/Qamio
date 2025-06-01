include ../../Make.inc



# Config

Module = lib.qaos.paramparser

Srcs = \
	Dev/ParamParser.cpp \

Dirs = \
	$(QDK)/dev.qaos.basis/Inc \
	$(QDK)/dev.qaos.paramparser/Inc \


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

$(Out)/Package.moq: $(Lib)/libParamParser.so
	@echo "  - 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Lib



$(Lib)/libParamParser.so: $(Lib)/libParamParser.so.bc

	@echo "  - 🔨 libParamParser.so"

	@$(CP) -shared $(Lib)/libParamParser.so.bc \
		-o $(Lib)/libParamParser.so



$(Lib)/libParamParser.so.bc: $(Objs)
	@echo "  - 🔗 libParamParser.so.bc"
	@llvm-link $(Objs) \
		-o $(Lib)/libParamParser.so.bc



$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

