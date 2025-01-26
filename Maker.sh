# Def
set -e
set -o pipefail


# Lib
run() {
  echo "${P}$*"
  "$@"
}
Find () {
  echo "$(cat Conf/Compile.conf | grep -w "$1" | cut -d' ' -f2)"
}


# Qamio
MainVer=$(Find "MainVer")
SubVer=$(Find "SubVer")
Patch=$(Find "Patch")
Name=$(Find "Name")


# Const
P="  "


# Modes
Hello() {
  echo "Qamio Builder"
	echo "v${MainVer}.${SubVer}:${Patch}-${Name}"
  echo
}

Dispo() {
  echo "! Dispo"

  echo "${P}# Unmount files" 
  ls Dev/Mount | while read file; do
    run sudo umount Dev/Mount/"$file"
    run sudo rm -r Dev/Mount/"$file"
  done

  echo
  echo "${P}# Dispose loops"
  if [ -f "Dev/Temp/Loop.list" ]; then
    run sudo losetup -d $(cat "Dev/Temp/Loop.list")
    run rm "Dev/Temp/Loop.list"
  fi
  echo
}

Clean() {
  Dispo

  echo "! Clean"

    echo "${P}rm -rf Dev/Images/*"
  rm -rf "Dev/Images/"*

    echo "${P}rm -rf Dev/Temp/*"
  rm -rf "Dev/Temp/"*

  echo
}

Pack() {
  echo "! Pack"

  echo "${P}# Syncing images"
  run sync

  # Images
  echo "${P}# Compressing images"
  ls Dev/Images | while read file; do
    run xz -k "Dev/Images/$file"

    echo -n $(sha256sum "Dev/Images/$file" | cut -d' ' -f1) >> "Dev/Images/$file°sha256"
  done

  echo
}

Flash() {
  echo "! Flash"

  false
  # Fix it
  echo -n "${P}Device path: $WDev"
  #read WDev
  echo

  # Disk reset
  run sudo dd if=/dev/zero of="$WDev" bs=2048 count=1 conv=notrunc status=none

  run sudo parted "$WDev" mklabel gpt -s

  # Parts
  run sudo parted "$WDev" mkpart primary 2048s 1230847s -s
  run sudo parted "$WDev" mkpart primary 1230848s 9615359s -s
  run sudo parted "$WDev" mkpart primary 9615360s 60086271s -s

  echo
}

Start() {
  echo "! Sync"
  run sync
  echo

  Dispo


  echo "! Start"
  # full-screen=on
  run qemu-system-x86_64 \
  -enable-kvm \
  -m 4G \
  -smp 8 \
  -drive file=Dev/Images/Initrd.img,format=raw \
  -drive file=Dev/Images/System.img,format=raw \
  -kernel Res/Linux_6.12.10.img \
  -vga virtio -display gtk,gl=on \
  -append "root=/dev/sda ro init=/Qiniter.elf console=ttyS0" \
  -serial stdio

  echo
}

Help() {
  echo "Usage: $(basename $0) .mode"

  echo "  .b .build    # Build all system"
  echo "  .d .dispo    # Dispose resorces"
  echo "  .c .clean    # Clear last build"
  echo "  .p .pack     # Pack images"
  echo "  .f .flash    # Flash to a device"
  echo "  .k .kernel   # Compile kernel"
  echo "  .m .modules  # Compile modules"
  echo "  .s .start    # Start with Qemu"
  echo "  .h .help     # Show this page"
  echo "  .v .version  # Print version"

  echo
}

Build() {
  Clean

  echo "! Creating Images"
  DiskC=$(crudini --get "Dev/Device.conf" Disk ListC)
  for ((i=1; i<=DiskC; i++)); do
    Part=$(crudini --get "Dev/Device.conf" Disk "List_$i")
    DiskSize=$(crudini --get "Dev/Device.conf" "Disk.$Part" Size)
    WDir="Dev/Mount/"$Part

    echo "${P}# $Part"
    run truncate -s $DiskSize "Dev/Images/$Part.img"
    run mkfs.ext4 -q "Dev/Images/$Part.img"
    
    # Push Loop
    CLoop=$(sudo losetup -f)
    echo "${CLoop}" >> "Dev/Temp/Loop.list"
    
    # Mount Disk
    run sudo losetup ${CLoop} "Dev/Images/$Part.img"
    run mkdir "$WDir"
    run sudo mount ${CLoop} "$WDir"
    run sudo chown -R $(whoami) "$WDir"
    echo
  done

  
  echo "! Build Basic"
	for file in $(bash Src/Pkg.conf $1); do
    # Package Time
		echo "> $file"
      bash "Src/$file/Maker.sh"
      echo
	done


  echo "! Build Package"
	for file in $(bash Pkg/Pkg.conf $1); do
    # Package Time
		echo "> $file"
      bash "Pkg/$file/Maker.sh"
      echo
	done
}


# Router
case "$1" in
  .b|.build)
    Hello
    Build "Build"
    ;;
  .k|.kernel)
    Hello
    Build "Kernel"
    ;;
  .m|.modules)
    Hello
    Build "Modules"
    ;;
  .d|.dispo)
    Hello
    Dispo
    ;;
  .c|.clean)
    Hello
    Clean
    ;;
  .p|.pack)
    Hello
    Pack
    ;;
  .f|.flash)
    Hello
    Flash
    ;;
  .s|.start)
    Hello
    Start
    ;;
  .h|.help)
    Help
    ;;
  .v|.version)
    Hello
    ;;
  *)
    echo "Unknown Command: $1"
    echo "Please use '.h | .help'"
    false
    ;;
esac
