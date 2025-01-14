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
  bash Tools/gcc.sh $*
}
moq() {
  bash Tools/moq.sh $*
}


# Const
P="  "
Self=$(dirname "$0")
Mach=$(Find "Architecture")


# Modes
build() {
  run GCC -shared "$Self/Src/Screen.c" -o Temp/qaos-Screen.so

  moq .n "qaos-Screen"
  moq .a "qaos-Screen" Nuc Temp/qaos-Screen.so
  moq .p "qaos-Screen"
  run moq .i "qaos-Screen"
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
