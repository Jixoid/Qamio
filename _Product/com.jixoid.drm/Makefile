include ../../Make.inc



# Targets

Build: Basis $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)


Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq:
	@echo "  - 🗜️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C . Mod  -C . Lib
