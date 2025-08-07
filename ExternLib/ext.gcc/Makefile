include ../../Make.inc


VER   	:= $$(curl -s https://ftp.gnu.org/gnu/gcc/ | grep -oP 'gcc-\K[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1)
TARBALL := $(Out)/gcc.tar.xz
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

$(STAMP): $(TARBALL)
	@tar -xf $(TARBALL) -C $(Out)

	@echo "  - 🔨 Compiling (-j8)"
	
	@konsole -e bash -c " \
		cd $(Out) && cd gcc-* && \
		\
		./contrib/download_prerequisites && \
		\
		mkdir -p build && \
		cd build && \
		\
		../configure --disable-multilib --enable-languages=c,c++ --prefix=$$(pwd) &&\
		make -j8 && \
		\
		cd ../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi

	@cd $(Out) && cd gcc-* && \
		find . -type f,l  -name 'libstdc++.so*'  -exec cp {} ../../$(Lib) \; && \
		find . -type f,l  -name 'libgcc_s.so*'  -exec cp {} ../../$(Lib) \; && \
		find . -type f,l  -name 'libatomic.so*'  -exec cp {} ../../$(Lib) \; && \
		find . -type f,l  -name 'libitm.so*'  -exec cp {} ../../$(Lib) \; && \
		find . -type f,l  -name 'libquadmath.so*'  -exec cp {} ../../$(Lib) \;
		
		
	@echo


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "  - 🌐 https://ftp.gnu.org/gnu/gcc"
	@echo "  - 🌟 Thanks to gnu.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e "  - ⬇️ Downloading\033[1A"; \
		\
		echo "  - ⬇️ Downloading gcc-$(VER)"; \
		curl --progress-bar -L https://ftp.gnu.org/gnu/gcc/gcc-$(VER)/gcc-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\033[1A\033[2K"; \
	fi

