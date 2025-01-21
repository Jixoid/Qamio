# Def
set -e
set -o pipefail


# Const
Part="System"
P="  "


# Variables
WDir="Dev/Mount/"$Part


# Lib
run() {
  echo "${P}$*"
  "$@"
}


# Modes
build() {
    echo "${P}mkdir {System}/{Moq,Qap,Pkg,Conf,old_root,dev,proc,sys}"
  mkdir "$WDir"/{Moq,Qap,Pkg,Conf,old_root,dev,proc,sys}

  ln -s /old_root/etc   "$WDir"/etc
  ln -s /old_root/usr   "$WDir"/usr
  ln -s /old_root/lib   "$WDir"/lib
  ln -s /old_root/lib64 "$WDir"/lib64
  ln -s /old_root/bin   "$WDir"/bin
  ln -s /old_root/sbin  "$WDir"/sbin

  crudini --set Dev/Mount/System/Conf/Nucleol.conf Root ListC 0

  #  echo "${P}cp -r {Pkg}/Linux/* {Initrd}/Linux"
  #cp -r "$(dirname "$0")"/Linux/* "$WDir"/Linux
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
