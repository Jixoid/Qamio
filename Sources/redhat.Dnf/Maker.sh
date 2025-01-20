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


# Const
P="  "
Self=$(dirname "$0")
Mach=$(Find "Architecture")


# Modes
build() {
  #cp $(find Mount/Initrd/ -type f -name "*.rpm") Repo/
  #run sudo dnf install --installroot=$(pwd)/Mount/Initrd --use-host-config $(cat Sources/Extra.list)

    echo "${P}sudo dnf install -y --installroot=$(pwd)/Mount/Initrd Repo/*"
  sudo dnf install -y --installroot=$(pwd)/Mount/Initrd Repo/*
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
