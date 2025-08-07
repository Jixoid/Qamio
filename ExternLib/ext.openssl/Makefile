include ../../Make.inc


VER   	:= $$(curl -s https://openssl-library.org/source/ | grep -oP 'openssl-\K[0-9]+\.[0-9]+\.[0-9]+(?=\.tar\.gz)' | sort -V | tail -n1)
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
		./Configure && \
		make -j$(nproc) && \
		\
		cd ../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null

	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi

	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libcrypto.so*'  -exec cp {} $(Lib) \;
	@find $(DIR) -type f,l  ! -name '*.symbols' -name 'libssl.so*'  -exec cp {} $(Lib) \;

	@touch $(STAMP)
	@echo


$(DIR):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "  - 🌐 https://openssl-library.org/source/"
	@echo "  - 🌟 Thanks to openssl-library.org"

	@set -e; \
		if [ ! -f "$(DIR)" ]; then \
			echo -e "  - ⬇️ Downloading\033[1A"; \
			\
			echo "  - ⬇️ Downloading openssl-$(VER)"; \
			git clone https://github.com/openssl/openssl.git --depth=1 $(DIR); \
		fi

