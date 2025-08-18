include ../../Make.inc


VER   	:= $$(curl -s https://ftp.gnu.org/gnu/glibc/ | grep -oP 'glibc-\K[0-9]+\.[0-9]+(?=\.tar\.xz)' | sort -V | tail -n1)
TARBALL := $(Out)/glibc.tar.xz
DIR     := $(Out)/glibc
STAMP   := $(Out)/glibc.stamp


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
		make -j$(nproc) && \
		\
		cd ../../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi


	find $(DIR)/build -type f,l  -name 'lib*.so*'  -exec cp {} $(Lib) \;
	cp   $(DIR)/build/elf/ld.so  $(Lib)/ld-qamio.elf
	
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
		../configure --prefix $$(pwd) \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "  - 🌐 https://ftp.gnu.org/gnu/glibc"
	@echo "  - 🌟 Thanks to gnu.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e "  - ⬇️ Downloading\033[1A"; \
		\
		echo "  - ⬇️ Downloading glibc-$(VER)"; \
		curl --progress-bar -L https://ftp.gnu.org/gnu/glibc/glibc-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\033[1A\033[2K"; \
	fi
