include ../../Make.inc


# Config

Module = com.qaos.jconf

Srcs = \
	Dev/JConf.cpp \

Dirs = \
	$(SYS)/com.qaos.basis-devel/Inc \
	$(SYS)/com.qaos.jconf-devel/Inc


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

$(Out)/Package.moq: $(Lib)/libJConf.so $(Lib)/JConf.o
	@echo "- 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Lib



$(Lib)/libJConf.so: $(Lib)/JConf.o.bc
	@echo "- 🔨 libJConf.so"
	@$(CP) -shared $(Lib)/JConf.o.bc \
		-o $(Lib)/libJConf.so

$(Lib)/JConf.o: $(Lib)/JConf.o.bc
	@echo "- 🔨 JConf.o"
	@$(CP) -c $(Lib)/JConf.o.bc \
		-o $(Lib)/JConf.o


$(Lib)/JConf.o.bc: $(Tmp)/JConf.bc
	@echo "- 🔗 JConf.o.bc"
	@llvm-link $(Tmp)/JConf.bc \
		-o $(Lib)/JConf.o.bc


$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "- ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

