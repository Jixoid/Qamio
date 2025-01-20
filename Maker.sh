# Def
set -e
set -o pipefail


# Lib
run() {
  echo "${P}$*"
  "$@"
}
Find () {
  echo "$(cat Config/Compile.conf | grep "$1" | cut -d' ' -f2)"
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
  ls Mount | while read file; do
    run sudo umount Mount/"$file"
    run sudo rm -r Mount/"$file"
  done

  echo
  echo "${P}# Dispose loops"
  if [ -f "Temp/Loop.list" ]; then
    run sudo losetup -d $(cat "Temp/Loop.list")
    run rm "Temp/Loop.list"
  fi
  echo
}

Clean() {
  Dispo

  echo "! Clean"

    echo "${P}rm -rf Images/*"
  rm -rf "Images/"*

    echo "${P}find Core -type f -exec rm {} \;"
  find Core -type f -exec rm {} \;

    echo "${P}rm -rf Logs/*"
  rm -rf "Logs/"*

    echo "${P}rm -rf Temp/*"
  rm -rf "Temp/"*

  echo
}

Pack() {
  echo "! Pack"

  echo "${P}# Syncing images"
  run sync

  # Images
  echo "${P}# Compressing images"
  ls ./Images | while read file; do
    run xz -k "Images/$file"
    echo -n $(sha256sum "Images/$file" | cut -d' ' -f1) >> "Images/$file°sha256"
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
  -drive file=Images/Initrd.img,format=raw \
  -drive file=Images/System.img,format=raw \
  -initrd Images/Initrd.img \
  -kernel Resources/Linux_6.12.9.img \
  -vga std -display sdl \
  -append "root=/dev/sda ro init=/bin/bash console=ttyS0 vga=788" \
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
  cat Config/DiskTable.tbl | cut -d' ' -f1 | while read disk; do
    Part=$disk
    WDir="Mount/"$Part

    echo "${P}# $Part"
    # Create Disk
    DiskSize=$(cat Config/DiskTable.tbl | grep "$Part" | cut -d' ' -f2)
    run truncate -s $DiskSize "Images/$Part.img"
    run mkfs.ext4 -q "Images/$Part.img"
    
    # Push Loop
    CLoop=$(sudo losetup -f)
    echo "${CLoop}" >> "Temp/Loop.list"
    
    # Mount Disk
    run sudo losetup ${CLoop} "Images/$Part.img"
    run mkdir "$WDir"
    run sudo mount ${CLoop} "$WDir"
    run sudo chown -R $(whoami) "$WDir"
    echo
  done

  
  echo "! Build"
	for file in $(bash Sources/Pkg.conf $1); do
    # Package Time
		echo "> $file"
      bash "Sources/$file/Maker.sh"
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
