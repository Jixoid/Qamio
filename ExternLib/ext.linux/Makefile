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
	@echo -e $(L_BLD) "$(notdir $(DIR)) $(GRAY)(-j$(shell nproc))$(RESET)"
	
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
	@echo -e $(L_EXT) "$(notdir $(TARBALL))"
	@tar -xf $(TARBALL) -C $(Out)
	@mv $(DIR)-* $(DIR)

	@echo -e $(L_CFG) "$(notdir $(DIR))"
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

	@echo "    🌐 https://www.kernel.org/pub/linux/kernel/v6.x"
	@echo "    🌟 Thanks to kernel.org"
	
	@set -e; \
	if [ ! -f "$(TARBALL)" ]; then \
		echo -e $(L_DWL) "linux \e[1A"; \
		\
		echo -e $(L_DWL) "linux-$(VER).tar.xz"; \
		curl --progress-bar -L https://www.kernel.org/pub/linux/kernel/v6.x/linux-$(VER).tar.xz -o "$(TARBALL)"; \
		\
		echo -ne "\e[1A\e[2K"; \
	fi

