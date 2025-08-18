include ../../Make.inc


VER   	:= $$(curl -s https://dri.freedesktop.org/libdrm/ | grep -oP 'libdrm-\K[0-9]+\.[0-9]+\.[0-9]+(?=\.tar\.xz)' | sort -V | tail -n1)
TARBALL := $(Out)/libdrm.tar.xz
DIR     := $(Out)/libdrm
STAMP   := $(Out)/libdrm.stamp


# Targets

Build: Basis $(STAMP)


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files
$(STAMP): $(DIR)
	@echo "  - 🔨 Compiling (-j$(shell nproc))"
	@konsole -e bash -c " \
		cd $(DIR)/build && \
		\
		ninja -j $$(nproc) && \
		\
		cd ../../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi

	@find $(DIR)/build -type f,l  ! -name '*.symbols' -name '*.so*'  -exec cp {} $(Lib) \;

	@echo


$(DIR): $(TARBALL)
	@echo "  - ⚙️ Extract and Configure"

	@tar -xf $(TARBALL) -C $(Out)

	@mv $(DIR)-* $(DIR)

	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		mkdir -p build && \
		cd build && \
		\
		meson setup \
			--buildtype=release   \
			-D udev=true          \
			-D valgrind=disabled  \
			.. \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "  - 🌐 https://dri.freedesktop.org/libdrm"
	@echo "  - 🌟 Thanks to freedesktop.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e "  - ⬇️ Downloading\033[1A"; \
		\
		echo "  - ⬇️ Downloading libdrm-$(VER)"; \
		curl --progress-bar -L https://dri.freedesktop.org/libdrm/libdrm-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\033[1A\033[2K"; \
	fi
