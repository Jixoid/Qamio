include ../../Make.inc


VER   	:= $$(curl -s https://ftp.gnu.org/gnu/glibc/ | grep -oP 'glibc-\K[0-9]+\.[0-9]+(?=\.tar\.xz)' | sort -V | tail -n1)
TARBALL := $(GOut)/glibc.tar.xz
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
	@echo -e $(L_BLD) "$(notdir $(DIR)) $(GRAY)(-j$(shell nproc))$(RESET)"

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
	@echo -e $(L_EXT) "$(notdir $(TARBALL))"
	@tar -xf $(TARBALL) -C $(Out)
	@mv $(DIR)-* $(DIR)

	@echo -e $(L_CFG) "$(notdir $(DIR))"
	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		mkdir -p build && \
		cd build && \
		\
		../configure \
			--host=$(CTarget)-linux-gnu \
			--prefix $$(pwd) \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "    🌐 https://ftp.gnu.org/gnu/glibc"
	@echo "    🌟 Thanks to gnu.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e $(L_DWL) "glibc \e[1A"; \
		\
		echo -e $(L_DWL) "glibc-$(VER).tar.xz"; \
		curl --progress-bar -L https://ftp.gnu.org/gnu/glibc/glibc-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\e[1A\e[2K"; \
	fi
