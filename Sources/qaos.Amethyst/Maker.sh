# Def
set -e
set -o pipefail


# Lib
run() {
  echo "${P}$*"
  "$@"
}
Find () {
  echo "$(cat Config/Compile.conf | grep -w "$1" | cut -d' ' -f2)"
}
GCC() {
  echo -n $(bash Tools/gcc.sh $*)
}
moq() {
  bash Tools/moq.sh $*
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
  run GCC "$Self/Src/Amethyst.c" Core/$Mach/Static/* -o Temp/qaos.Amethyst.elf \
  "-DMainVer=$MainVer" "-DSubVer=$SubVer" "-DPatchVer=$Patch" "-DNameVer=\"$Name\""

  moq .n "qaos.Amethyst"
  moq .a "qaos.Amethyst" Bin Temp/qaos.Amethyst.elf
  moq .p "qaos.Amethyst"
  run moq .i "qaos.Amethyst"

  echo -n "qaos.Amethyst" >> "Mount/System/Conf/Kernel.go"
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
