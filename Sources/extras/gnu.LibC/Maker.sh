# Def
set -e
set -o pipefail


# Const
P="  "


# Variables
WDir="Mount/Initrd"


# Lib
run() {
  echo "${P}$*"
  "$@"
}


# Modes
build() {
    echo "${P}cp -r {Pkg}/usr/* {Initrd}/usr"
  cp -r "$(dirname "$0")"/usr/* "$WDir"/usr
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
