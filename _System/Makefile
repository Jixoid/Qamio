include ../Make.inc


# Config

Plan = $(shell ls | grep "^!com.")
Srcs = $(shell ls | grep "^lib.") $(shell ls | grep "^com.")



# Targets
	
Build: Basis

	@set -e; \
	for target in $(Plan); do \
		echo -e "  ⭐ $(ORANGE)$$target$(RESET)"; \
	done


	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🧩 $(ORANGE)$$target$(RESET)"; \
		make -C $$target Build; \
		\
		if [ -f "$$target/.noinstall" ]; then \
    	continue; \
		fi; \
		\
		if [ ! -f "$$target/$(Out)/Package.moq" ]; then \
    	echo "❌ Paket dosyası durumlanamadı"; \
			rm -rf $(Out)/Moq/$$target; \
    	exit 1; \
		fi; \
		\
		if [ "$(Out)/Moq/$$target" -nt "$$target/$(Out)/Package.moq" ]; then \
			continue; \
		fi; \
		\
		echo -e "  - ⚡ Installing"; \
		rm -rf $(Out)/Moq/$$target; \
		mkdir $(Out)/Moq/$$target; \
		tar -xf $$target/$(Out)/Package.moq  -C $(Out)/Moq/$$target; \
	done

	@rm -rf $(Out)/Conf
	@mkdir -p $(Out)/Conf
	@cp -r _Conf/*  $(Out)/Conf

	@rm -f $(Out)/Conf/Init.elf
	@ln -s ../Moq/com.qaos.amethyst/Bin/Main.elf  $(Out)/Conf/Init.elf



Basis:
	@mkdir -p $(Out)/Moq
	@mkdir -p $(Out)/Conf




Clean:
	@rm -rf $(Out)

	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🗑️ $(ORANGE)$$target$(RESET)"; \
		make -C $$target Clean; \
	done

