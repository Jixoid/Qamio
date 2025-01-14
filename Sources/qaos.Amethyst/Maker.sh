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
GCC() {
  echo -n $(bash Tools/gcc.sh $*)
}


# Const
P="  "
Self=$(dirname "$0")
Mach=$(Find "Architecture")

MainVer=$(Find "MainVer")
SubVer=$(Find "SubVer")
Patch=$(Find "Patch")
Name=$(Find "Name")


# Modes
build() {
  run GCC "$Self/Src/Amethyst.c" Core/$Mach/Static/* -o Core/$Mach/Amethyst.elf \
  "-DMainVer=$MainVer" "-DSubVer=$SubVer" "-DPatchVer=$Patch" "-DNameVer=\"$Name\""
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
