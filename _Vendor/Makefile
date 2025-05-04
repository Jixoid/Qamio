include ../Make.inc


# Config

Srcs = \
	com.jixui.widget \
	\
	com.jixui.font.inter




# Targets
	
Build: Basis

	@set -e; \
	for target in $(Srcs); do \
		echo -e "🧩 $(BLUE)vendor$(RESET)::$(ORANGE)$$target$(RESET)"; \
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
		echo -e "- ⚡ Installing"; \
		rm -rf $(Out)/Moq/$$target; \
		mkdir $(Out)/Moq/$$target; \
		tar -xf $$target/$(Out)/Package.moq  -C $(Out)/Moq/$$target; \
	done

	@rm -rf $(Out)/Conf
	@mkdir -p $(Out)/Conf
	@cp -r _Conf/*  $(Out)/Conf



Basis:
	@mkdir -p $(Out)/Moq
	@mkdir -p $(Out)/Conf




Clean:
	@rm -rf $(Out)

	@set -e; \
	for target in $(Srcs); do \
		echo -e "🗑️ $(BLUE)vendor$(RESET)::$(ORANGE)$$target$(RESET)"; \
		make -C $$target Clean; \
	done

