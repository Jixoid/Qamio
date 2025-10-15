include ../../Make.inc


VER   	:= $$(curl -s https://github.com/xkbcommon/libxkbcommon/releases | grep -oP 'xkbcommon-\K[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1)
TARBALL := $(GOut)/libxkbcommon.tar.gz
DIR     := $(Out)/libxkbcommon
STAMP   := $(Out)/libxkbcommon.stamp


# Targets

Build: Basis $(STAMP)


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files
$(STAMP): $(DIR)
	@echo -e $(L_BLD) "$(notdir $(DIR)) $(GRAY)(-j$(shell nproc))$(RESET)"
	
	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		ninja -C build && \
		\
		cd ../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null

	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi

	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libxkbcommon.so*'  -exec cp {} $(Lib) \;

	@touch $(STAMP)
	@echo


$(DIR): $(TARBALL)
	@echo -e $(L_EXT) "$(notdir $(TARBALL))"
	@tar -xf $(TARBALL) -C $(Out)
	@mv $(DIR)-* $(DIR)

	@echo -e $(L_CFG) "$(notdir $(DIR))"
	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		meson setup build \
			-Ddefault_library=shared \
			-Denable-x11=false \
			-Denable-docs=false \
			-Denable-wayland=false \
			-Denable-tools=false \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "    🌐 https://xkbcommon.org"
	@echo "    🌟 Thanks to xkbcommon.org"

	@set -e; \
		if [ ! -f "$(DIR)" ]; then \
			echo -e $(L_DWL) "xkbcommon \e[1A"; \
			\
			echo -e $(L_DWL) "xkbcommon-$(VER).tar.gz"; \
			curl --progress-bar -L https://github.com/xkbcommon/libxkbcommon/archive/refs/tags/xkbcommon-$(VER).tar.gz -o "$(TARBALL)"; \
			\
			echo -ne "\e[1A\e[2K"; \
		fi

