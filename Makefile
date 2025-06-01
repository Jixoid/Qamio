include Make.inc


SU = sudo


include .config
export $(shell sed '/^\s*#/d;/^\s*$$/d;s/=.*//' .config)


CONFIG = $(shell Tools/ParseConfig.sh)
export CONFIG



.PHONY: Config
Config:
	@Tools/mconf Config/Main.kconfig



Compile:
	@echo "♦️ Compile"
	@echo


	@echo -e "💠 $(BLUE)qdk$(RESET): $(shell cat _QDK/_Config/Head.txt)"
	@$(MAKE) -C _QDK  Build
	@echo

	@echo -e "💠 $(BLUE)system$(RESET): $(shell cat _System/_Config/Head.txt)"
	@$(MAKE) -C _System  Build
	@echo
	
	@echo -e "💠 $(BLUE)vendor$(RESET): $(shell cat _Vendor/_Config/Head.txt)"
	@$(MAKE) -C _Vendor  Build
	@echo

	@echo -e "💠 $(BLUE)product$(RESET): $(shell cat _Product/_Config/Head.txt)"
	@$(MAKE) -C _Product Build
	@echo

	@echo -e "💠 $(BLUE)data$(RESET)"
	@$(MAKE) -C _Data    Build
	@echo

	@echo "Successfuly Compiled!"
	@echo
	@echo


Clean:
	@echo "♦️ Clean"
	@echo

	@echo -e "💠 $(BLUE)qdk$(RESET): $(shell cat _QDK/_Config/Head.txt)"
	@$(MAKE) -C _QDK  Clean
	@echo

	@echo -e "💠 $(BLUE)system$(RESET): $(shell cat _System/_Config/Head.txt)"
	@$(MAKE) -C _System  Clean
	@echo
	
	@echo -e "💠 $(BLUE)vendor$(RESET): $(shell cat _Vendor/_Config/Head.txt)"
	@$(MAKE) -C _Vendor  Clean
	@echo

	@echo -e "💠 $(BLUE)product$(RESET): $(shell cat _Product/_Config/Head.txt)"
	@$(MAKE) -C _Product Clean
	@echo

	@echo -e "💠 $(BLUE)data$(RESET)"
	@$(MAKE) -C _Data    Clean
	@echo

	@echo



DISK = Dev/Mount/Disk1
Build:
	@echo "♦️ Build"
	@echo


# Clean up
	@echo "🗑️ Clean up"

	@rm -rf $(DISK)/*



# Linux Basis
	@echo "💠 Linux Basis"

	@mkdir $(DISK)/Dev
	@mkdir $(DISK)/Proc
	@mkdir $(DISK)/Sys



# Cache
	@mkdir $(DISK)/Cache
	@mkdir $(DISK)/Cache/Font



# Flashing Basis
	@echo "🔥 Flashing Parts"

	@echo "  ⚡ System"
	@mkdir $(DISK)/System
	@cp -r _System/@Out/*   $(DISK)/System

	@echo "  ⚡ Vendor"
	@mkdir $(DISK)/Vendor
	@cp -r _Vendor/@Out/*   $(DISK)/Vendor

	@echo "  ⚡ Product"
	@mkdir $(DISK)/Product
	@cp -r _Product/@Out/*  $(DISK)/Product

	@echo "  ⚡ Data"
	@mkdir $(DISK)/Data
	@cp -r _Data/@Out/*     $(DISK)/Data

	@echo


# Bootstrap Lib
	@echo "🔗 Bootstrap"

	@ln -s System/Moq/com.qaos.bootstrap/Lib $(DISK)/Lib

	@echo
	@echo



Mount:
	@echo "♦️ Mount"
	@echo


# Disk1
	@echo "💠 Disk1.img"

	@mkdir -p $(DISK)
	
	@rm -f Dev/Image/Disk1.img
	@truncate -s $$(jq -r .Disk1.Size Conf/Disk.json)  Dev/Image/Disk1.img

	@mkfs.ext4  -O casefold  Dev/Image/Disk1.img &> /dev/null

	@$(SU) mount Dev/Image/Disk1.img $(DISK)
	@$(SU) chown alforce -R $(DISK)

	@rm -rf $(DISK)/*
	@chattr +F $(DISK)

	@echo
	@echo



UMount:
	@echo "♦️ UMount"
	@echo


# Sync
	@echo "✴️ Syncing"
	@sync


# Disk1
	@echo "💠 Disk1.img"
	
	@$(SU) umount $(DISK)

	@rm -r $(DISK)

	@echo



Start:
	@echo "qamio emulator"
	@echo

	@qemu-system-x86_64 \
		\
		-accel kvm \
		-cpu host \
		\
		-smp 6 \
		-m 6G \
		-usb \
		\
		-kernel Dev/Linux.elf \
		-append "root=/dev/sda rw  selinux=0  init=/System/Conf/Init.elf  console=ttyS0  quiet" \
		\
		-drive file=Dev/Image/Disk1.img,format=raw \
		-device virtio-gpu \
		-netdev user,id=net0 -device e1000,netdev=net0 \
		-device usb-host,vendorid=0x03f0,productid=0x5341 \
		\
		-display sdl \
		-serial stdio \


#	-device ich9-intel-hda -device hda-output
#	-device usb-mouse -device usb-kbd


FastBuild: Compile Mount Build UMount


PopLog:
	@echo "qamio emulater"
	@echo

	@rm -rf Log


	@echo "> Sync"
	@sync


	@echo "> Connect Qamio"
	@mkdir -p $(DISK)
	@$(SU) mount Dev/Image/Disk1.img $(DISK)


	@echo "> Poping Logs"
	@cp -r $(DISK)/Data/Log .
	
	@rm -rf $(DISK)/Data/Log
	@mkdir $(DISK)/Data/Log
	

	@echo "> Disconnect Qamio"
	@$(SU) umount Dev/Mount/Disk1
	@rm -r $(DISK)

