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
	@echo "qamio builder"
	@echo "Mode: Compile"
	@echo

	@$(MAKE) -C _System  Build
	@$(MAKE) -C _Vendor  Build
	@$(MAKE) -C _Product Build
	@$(MAKE) -C _Data    Build

	@echo
	@echo "Successfuly compiled!"



Clean:
	@echo "qamio builder"
	@echo "Mode: Clean"
	@echo

	@$(MAKE) -C _System  Clean
	@$(MAKE) -C _Vendor  Clean
	@$(MAKE) -C _Product Clean
	@$(MAKE) -C _Data    Clean



DISK = Dev/Mount/System
Build:
	@echo "qamio builder"
	@echo "Mode: Build"
	@echo


# Clean up
	@echo "> Clean up"

	@rm -rf $(DISK)/*



# Linux Basis
	@echo "> Linux Basis"

	@mkdir $(DISK)/Dev
	@mkdir $(DISK)/Proc
	@mkdir $(DISK)/Sys



# Cache
	@mkdir $(DISK)/Cache
	@mkdir $(DISK)/Cache/Font



# Flashing Basis
	@echo "> Flashing Parts"

	@mkdir $(DISK)/System
	@cp -r _System/@Out/*   $(DISK)/System

	@mkdir $(DISK)/Vendor
	@cp -r _Vendor/@Out/*   $(DISK)/Vendor

	@mkdir $(DISK)/Product
	@cp -r _Product/@Out/*  $(DISK)/Product

	@mkdir $(DISK)/Data
	@cp -r _Data/@Out/*     $(DISK)/Data


# Bootstrap Lib
	@echo "> Bootstrap"

	@ln -s System/Moq/com.qaos.bootstrap/Lib $(DISK)/Lib



Mount:
	@echo "qamio builder"
	@echo "Mode: Mount"
	@echo


# System
	@echo "> System.img"

	@mkdir -p $(DISK)
	
	@rm -f Dev/Image/System.img
	@truncate -s $$(jq -r .System.Size Conf/Disk.json)  Dev/Image/System.img

	@mkfs.ext4  -O casefold  Dev/Image/System.img

	@$(SU) mount Dev/Image/System.img $(DISK)
	@$(SU) chown alforce -R $(DISK)

	@rm -rf $(DISK)/*
	@chattr +F $(DISK)



UMount:
	@echo "qamio builder"
	@echo "Mode: UMount"
	@echo


# Sync
	@echo "> Syncing"
	@sync


# System
	@echo "> System.img"
	
	@$(SU) umount $(DISK)

	@rm -r $(DISK)



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
		-drive file=Dev/Image/System.img,format=raw \
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
	@$(SU) mount Dev/Image/System.img $(DISK)


	@echo "> Poping Logs"
	@cp -r $(DISK)/Data/Log .
	
	@rm -rf $(DISK)/Data/Log
	@mkdir $(DISK)/Data/Log
	

	@echo "> Disconnect Qamio"
	@$(SU) umount Dev/Mount/System
	@rm -r $(DISK)

