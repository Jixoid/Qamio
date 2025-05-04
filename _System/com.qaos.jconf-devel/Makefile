include ../../Make.inc



# Targets

Build: Basis  $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: Inc/JConf.hpp
	@echo "- 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C . Inc  -C @Out Lib


