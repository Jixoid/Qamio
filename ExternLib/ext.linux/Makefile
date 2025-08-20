include ../../Make.inc


VER   	:= $$(curl -s https://www.kernel.org/pub/linux/kernel/v6.x/ | grep -oP 'linux-\K[0-9]+\.[0-9]+\.[0-9]+' | sort -V | tail -n1)
TARBALL := $(GOut)/linux.tar.xz
DIR     := $(Out)/linux
STAMP   := $(Out)/linux.stamp


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
		make -j$$(nproc) && \
		\
		cd ../../../ && touch $(STAMP) \
		\
	  || { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


	@set -e; \
	if [ ! -f "$(STAMP)" ]; then \
		false; \
	fi


	@cp $$(find $(DIR) -type f -name bzImage)  $(Locate)/!Dev/Linux-$(Arch).elf
	@find $(DIR) -type f -name "*.ko" -exec cp {} $(Lib)/ \;
		
	@echo


$(DIR): $(TARBALL)
	@echo "  - ⚙️ Extract and Configure"

	@tar -xf $(TARBALL) -C $(Out)

	@mv $(DIR)-* $(DIR)

	@konsole -e bash -c " \
		cd $(DIR) && \
		\
		cp $(Locate)/!Dev/Linux.kconfig ./.config && \
		make menuconfig \
		\
		|| { echo -n 'Press any key to close... '; read -n 1; exit 1; } " &> /dev/null


$(TARBALL):
	@rm -rf $(Out)
	@mkdir -p $(Out)
	@mkdir -p $(Lib)

	@echo "  - 🌐 https://www.kernel.org/pub/linux/kernel/v6.x"
	@echo "  - 🌟 Thanks to kernel.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e "  - ⬇️ Downloading\033[1A"; \
		\
		echo "  - ⬇️ Downloading linux-$(VER)"; \
		curl --progress-bar -L https://www.kernel.org/pub/linux/kernel/v6.x/linux-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\033[1A\033[2K"; \
	fi

