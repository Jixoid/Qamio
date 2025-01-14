# Def
set -e
set -o pipefail


# Const
Part="System"
P="  "


# Variables
WDir="Mount/"$Part


# Lib
run() {
  echo "${P}$*"
  "$@"
}


# Modes
build() {
    echo "${P}mkdir {System}/{Packages,Conf}"
  mkdir "$WDir"/{Packages,Conf}
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
