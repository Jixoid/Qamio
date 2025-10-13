include ../../Make.inc


VER   	:= $$(curl -s https://ftp.gnu.org/gnu/gcc/ | grep -oP 'gcc-\K[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1)
TARBALL := $(GOut)/gcc.tar.xz
DIR     := $(Out)/gcc
STAMP   := $(Out)/gcc.stamp


# Targets

Build: Basis $(STAMP)


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files
$(STAMP): $(DIR)
	@echo -e $(L_BLD) "$(notdir $(DIR)) $(GRAY)(-j8)$(RESET)"
	
	@konsole -e bash -c " \
		cd $(DIR)/build && \
		\
		make -j8 && \
		\
		cd ../../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi

	@find $(DIR)/build -type f,l  -name 'libstdc++.so*'   -exec cp {} $(Lib) \;
	@find $(DIR)/build -type f,l  -name 'libstdc++.a*' 		-exec cp {} $(Lib) \;
	@find $(DIR)/build -type f,l  -name 'libgcc_s.so*'  	-exec cp {} $(Lib) \;
	@find $(DIR)/build -type f,l  -name 'libatomic.so*'  	-exec cp {} $(Lib) \;
	@find $(DIR)/build -type f,l  -name 'libitm.so*'  		-exec cp {} $(Lib) \;
	@find $(DIR)/build -type f,l  -name 'libquadmath.so*' -exec cp {} $(Lib) \;
		
	@echo


$(DIR): $(TARBALL)
	@echo -e $(L_EXT) "$(notdir $(TARBALL))"
	@tar -xf $(TARBALL) -C $(Out)
	@mv $(DIR)-* $(DIR)

	@echo -e $(L_CFG) "$(notdir $(DIR))"
	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		./contrib/download_prerequisites && \
		mkdir -p build && \
		cd build && \
		\
		../configure \
			--target=$(CTarget)-linux-gnu \
			--disable-multili \
			--enable-languages=c,c++ \
			--prefix=$$(pwd) \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "    🌐 https://ftp.gnu.org/gnu/gcc"
	@echo "    🌟 Thanks to gnu.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e $(L_DWL) "gcc \e[1A"; \
		\
		echo -e $(L_DWL) "gcc-$(VER).tar.xz"; \
		curl --progress-bar -L https://ftp.gnu.org/gnu/gcc/gcc-$(VER)/gcc-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\e[1A\e[2K"; \
	fi

