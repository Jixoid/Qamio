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
    echo "${P}mkdir {System}/{Moq,Qap,Pkg,Conf,Linux}"
  mkdir "$WDir"/{Moq,Qap,Pkg,Conf,Linux,Linux/etc}

  ln -s Linux/etc "$WDir"/etc

  #  echo "${P}cp -r {Pkg}/Linux/* {Initrd}/Linux"
  #cp -r "$(dirname "$0")"/Linux/* "$WDir"/Linux
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
