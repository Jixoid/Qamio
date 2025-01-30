# Def
set -e
set -o pipefail


# Const
Self=$(dirname "$0")
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
  
    echo "${P}mkdir {System}/{Dev,Proc,Sys,Initrd}"
  mkdir "$WDir"/{Dev,Proc,Sys,Initrd}


    echo "${P}mkdir {System}/{Moq,Pkg,Conf,User}"
  mkdir "$WDir"/{Moq,Pkg,Conf,User}


    echo "${P}"cp {Pkg}/Dev/Device.conf {System}/Conf
  cp Dev/Device.conf "$WDir"/Conf


  crudini --set Dev/Mount/System/Conf/Nucleol.conf Root ListC 0
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
