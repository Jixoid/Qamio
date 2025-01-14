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


# Modes
Hello() {
  echo "Qamio Builder"
	echo "v${MainVer}.${SubVer}:${Patch}-${Name}"
  echo
}

Build() {
	for file in $(bash Sources/Pkg.conf $1); do
    # Parse Name
		filename=$(basename "$file")

    # Package Time
		echo "> $filename"
      bash "Sources/$file/Maker.sh"
      echo
	done
}

Start() {
  run sync

  Build "Dispo"

  # full-screen=on
  qemu-system-x86_64 \
  -enable-kvm \
  -m 4G \
  -smp 8 \
  -drive file=Images/Initrd.img,format=raw \
  -drive file=Images/System.img,format=raw \
  -initrd Images/Initrd.img \
  -kernel Resources/vmlinuz-6.12.8.img \
  -vga std -display sdl,gl=on \
  -append "root=/dev/sda rw init=/bin/bash console=ttyS0 vga=788" \
  -serial stdio
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
}


# Router
case "$1" in
  .b|.build)
    Hello
    Build "Build"
    ;;
  .d|.dispo)
    Hello
    Build "Dispo"
    ;;
  .c|.clean)
    Hello
    Build "Clean"
    ;;
  .p|.pack)
    Hello
    Build "Pack"
    ;;
  .f|.flash)
    Hello
    Build "Flash"
    ;;
  .k|.kernel)
    Hello
    Build "Kernel"
    ;;
  .m|.modules)
    Hello
    Build "Modules"
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
