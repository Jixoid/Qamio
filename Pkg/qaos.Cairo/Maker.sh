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
GCC() {
  echo -n $(bash Tools/gcc.sh $*)
}
moq() {
  bash Tools/moq.sh $*
}


# Const
P="  "
Self=$(dirname "$0")


# Modes
build() {
  moq .n $(basename $Self)
  moq .a $(basename $Self)  "/" "$Self/Package.info"
  moq .a $(basename $Self)  "/" "$Self/Bin"
  moq .a $(basename $Self)  "/" "$Self/Lib"
  moq .a $(basename $Self)  "/" "$Self/Res"
  moq .p $(basename $Self)
  run moq .i $(basename $Self)
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
