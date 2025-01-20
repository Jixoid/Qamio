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


# Modes
build() {
  run GCC "$Self/Src/Qiniter.cpp" -o Mount/Initrd/Qiniter.elf
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
