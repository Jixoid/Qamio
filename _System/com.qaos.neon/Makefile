include ../../Make.inc


# Config

Module = com.qaos.neon

Srcs = \
	Dev/Neon.cpp \

Dirs = \
	$(QDK)/dev.qaos.basis/Inc \
	$(QDK)/dev.qaos.neon/Inc


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

$(Out)/Package.moq: $(Lib)/libNeon.so
	@echo "  - 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Lib



$(Lib)/libNeon.so: $(Lib)/libNeon.so.bc
	@echo "  - 🔨 libNeon.so"
	@$(CP) -shared $(Lib)/libNeon.so.bc \
		-o $(Lib)/libNeon.so



$(Lib)/libNeon.so.bc: $(Objs)
	@echo "  - 🔗 libNeon.so.bc"
	@llvm-link $(Objs) \
		-o $(Lib)/libNeon.so.bc



$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

