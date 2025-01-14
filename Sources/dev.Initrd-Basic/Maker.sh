# Def
set -e
set -o pipefail


# Const
Part="Initrd"
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
    echo "${P}mkdir {Initrd}/{usr,usr/bin,usr/lib,usr/lib64,usr/libexec,usr/etc}"
  mkdir "$WDir"/{usr,usr/bin,usr/lib,usr/lib64,usr/libexec,usr/etc}

  ln -s usr/bin "$WDir"/bin
  ln -s usr/lib "$WDir"/lib
  ln -s usr/lib64 "$WDir"/lib64
  ln -s usr/etc "$WDir"/etc

    echo "${P}cp {Pkg}/usr/etc/* {Initrd}/usr/etc"
  cp "$(dirname "$0")"/usr/etc/* "$WDir"/usr/etc
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
