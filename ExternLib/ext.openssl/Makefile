include ../../Make.inc


VER   	:= $$(curl -s https://openssl-library.org/source/ | grep -oP 'openssl-\K[0-9]+\.[0-9]+\.[0-9]+(?=\.tar\.gz)' | sort -V | tail -n1)
TARBALL := $(Out)/openssl.tar.gz
DIR     := $(Out)/openssl
STAMP   := $(Out)/openssl.stamp


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
		cd $(DIR) && \
		\
		make -j$(nproc) && \
		\
		cd ../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null

	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi

	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libcrypto.so*'  -exec cp {} $(Lib) \;
	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libssl.so*' 		 -exec cp {} $(Lib) \;

	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libcrypto.a*'  -exec cp {} $(Lib) \;
	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libssl.a*'  		-exec cp {} $(Lib) \;

	@touch $(STAMP)
	@echo


$(DIR): $(TARBALL)
	@echo "  - ⚙️ Extract and Configure"

	@tar -xf $(TARBALL) -C $(Out)

	@mv $(DIR)-* $(DIR)

	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		./Configure \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "  - 🌐 https://openssl-library.org/source"
	@echo "  - 🌟 Thanks to openssl-library.org"

	@set -e; \
		if [ ! -f "$(DIR)" ]; then \
			echo -e "  - ⬇️ Downloading\033[1A"; \
			\
			echo "  - ⬇️ Downloading openssl-$(VER)"; \
			curl --progress-bar -L https://github.com/openssl/openssl/releases/download/openssl-$(VER)/openssl-$(VER).tar.gz -o "$(TARBALL)"; \
			\
			echo -ne "\033[1A\033[2K"; \
		fi

