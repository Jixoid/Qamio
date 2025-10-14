#
#  This file is part of QAOS.
#
#  This file is licensed under the GNU Lesser General Public License version 3 (LGPLv3).
#
#  You should have received a copy of GNU Lesser General Public License
#  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
#
#  Copyright (c) 2025 by Kadir Aydın.
#



include .config
export $(shell sed '/^\s*#/d;/^\s*$$/d;s/=.*//' .config)

include Make.inc



.PHONY: Config
Config:
	@Tools/mconf '&Conf/Main.kconfig'



def.list: .config
	@echo "♦️ Reconfig"
	@echo

	@$(MAKE) Clean

	@Tools/ParseConfig.sh


Compile: def.list
	@echo "♦️ Compile  Target: $(Arch)"
	@echo


	@echo -e "🔗 $(BLUE)externs$(RESET): dependency packages"
	@$(MAKE) -C ExternLib    Build
	@echo

	@echo -e "📦 $(BLUE)qdk$(RESET): Qamio developer kit"
	@$(MAKE) -C QDK  Build
	@echo

	@echo -e "💠 $(BLUE)system$(RESET): $(shell cat 'Part/System/&Conf/Head.txt')"
	@$(MAKE) -C Part/System  Build
	@echo
	
	@echo -e "💠 $(BLUE)vendor$(RESET): $(shell cat 'Part/Vendor/&Conf/Head.txt')"
	@$(MAKE) -C Part/Vendor  Build
	@echo

	@echo -e "💠 $(BLUE)product$(RESET): $(shell cat 'Part/Product/&Conf/Head.txt')"
	@$(MAKE) -C Part/Product Build
	@echo

	@echo -e "💾 $(BLUE)data$(RESET): preloaded"
	@$(MAKE) -C Part/Data    Build
	@echo

	@echo "Successfuly Compiled!"
	@echo
	@echo


Clean:
	@echo "♦️ Clean"
	@echo


	@echo -e "📦 $(BLUE)qdk$(RESET): Qamio developer kit"
	@$(MAKE) -C QDK  Clean
	@echo

	@echo -e "💠 $(BLUE)system$(RESET): $(shell cat 'Part/System/&Conf/Head.txt')"
	@$(MAKE) -C Part/System  Clean
	@echo
	
	@echo -e "💠 $(BLUE)vendor$(RESET): $(shell cat 'Part/Vendor/&Conf/Head.txt')"
	@$(MAKE) -C Part/Vendor  Clean
	@echo

	@echo -e "💠 $(BLUE)product$(RESET): $(shell cat 'Part/Product/&Conf/Head.txt')"
	@$(MAKE) -C Part/Product Clean
	@echo

	@echo -e "💠 $(BLUE)data$(RESET)"
	@$(MAKE) -C Part/Data    Clean
	@echo

	@echo



CleanAll:
	@echo "♦️ Clean All"
	@echo


	@echo -e "🔗 $(BLUE)externs$(RESET): dependency packages"
	@$(MAKE) -C ExternLib    Clean
	@echo

	@echo -e "📦 $(BLUE)qdk$(RESET): Qamio developer kit"
	@$(MAKE) -C QDK  Clean
	@echo

	@echo -e "💠 $(BLUE)system$(RESET): $(shell cat 'Part/System/&Conf/Head.txt')"
	@$(MAKE) -C Part/System  Clean
	@echo
	
	@echo -e "💠 $(BLUE)vendor$(RESET): $(shell cat 'Part/Vendor/&Conf/Head.txt')"
	@$(MAKE) -C Part/Vendor  Clean
	@echo

	@echo -e "💠 $(BLUE)product$(RESET): $(shell cat 'Part/Product/&Conf/Head.txt')"
	@$(MAKE) -C Part/Product Clean
	@echo

	@echo -e "💠 $(BLUE)data$(RESET)"
	@$(MAKE) -C Part/Data    Clean
	@echo

	@echo



DISK = !Dev/Mount/Disk1
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



# Qamio Basis
	@echo "💠 Qamio Basis"

	@mkdir $(DISK)/System
	@mkdir $(DISK)/Vendor
	@mkdir $(DISK)/Product
	@mkdir $(DISK)/Data


	@mkdir $(DISK)/Keybox
	@mkdir $(DISK)/Slots
	@mkdir -p $(DISK)/Slots/System/main
	@mkdir -p $(DISK)/Slots/Vendor/main
	@mkdir -p $(DISK)/Slots/Product/main
	
	@mkdir $(DISK)/Neon
	@mkdir $(DISK)/Cache
	@mkdir $(DISK)/Cache/Font



# Flashing Basis
	@echo "🔥 Flashing Parts"

	@echo "  ⚡ System"
	@cp -r Part/System/$(Out)/*   $(DISK)/Slots/System/main

	@echo "  ⚡ Vendor"
	@cp -r Part/Vendor/$(Out)/*   $(DISK)/Slots/Vendor/main

	@echo "  ⚡ Product"
	@cp -r Part/Product/$(Out)/*  $(DISK)/Slots/Product/main

	@echo "  ⚡ Data"
	@cp -r Part/Data/$(Out)/*     $(DISK)/Data

	@echo


# Loading Qiniter
	@echo "🚀 Loading qiniter"
	@cp $(SYS)/com.qaos.qiniter/$(Bin)/Main.elf  $(DISK)/Qiniter.elf


# Loading keybox
	@echo "🔒 Loading keybox"
	@cp $(Locate)/!Dev/Key_public.pem  $(DISK)/Keybox/1.pem
	@echo

	@echo


Mount:
	@echo "♦️ Mount"
	@echo


# Disk1
	@echo "💠 Disk1.img"

	@mkdir -p $(DISK)
	
	@rm -f !Dev/Image/Disk1.img
	@truncate -s $$(jq -r .Disk1.Size !Dev/Disk.json)  !Dev/Image/Disk1.img

	@mkfs.ext4  -O casefold  !Dev/Image/Disk1.img -q

	@sudo mount !Dev/Image/Disk1.img $(DISK)
	@sudo chown alforce -R $(DISK)

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
	
	@sudo umount $(DISK)

	@rm -r $(DISK)

	@echo



Start:
	@echo "qamio emulator"
	@echo

	@qemu-system-$(CTarget) \
		\
		-accel kvm \
		-cpu host \
		\
		-smp 6 \
		-m 6G \
		-usb \
		\
		-kernel  !Dev/Linux-$(Arch).elf \
		-append "root=/dev/sda rw  selinux=0  init=/Qiniter.elf  console=ttyS0  loglevel=0" \
		\
		-drive file=!Dev/Image/Disk1.img,format=raw \
		-device virtio-gpu \
		-netdev user,id=net0 -device e1000,netdev=net0 \
		-device usb-host,vendorid=0x03f0,productid=0x5341 \
		\
		-display sdl \
		-serial stdio


#	-device ich9-intel-hda -device hda-output
#	-device usb-mouse -device usb-kbd


FastBuild: Compile Mount Build UMount


PopLog:
	@echo "qamio emulater"
	@echo

	@rm -rf !Dev/Log


	@echo "✴️ Sync"
	@sync


	@echo "🔗 Connect Qamio"
	@mkdir -p $(DISK)
	@sudo mount !Dev/Image/Disk1.img $(DISK)


	@echo "💾 Poping Logs"
	@cp -r $(DISK)/Data/Log !Dev
	
	@rm -rf $(DISK)/Data/Log
	@mkdir $(DISK)/Data/Log
	

	@echo "⛓️‍💥 Disconnect Qamio"
	@sudo umount !Dev/Mount/Disk1
	@rm -r $(DISK)

	@cat !Dev/Log/Qamio.log
	