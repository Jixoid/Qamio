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


# Const
P="  "
Self=$(dirname "$0")
Mach=$(Find "Architecture")


# Modes
build() {
  #cp $(find Dev/Mount/Initrd/ -type f -name "*.rpm") Dev/Repo/
  #run sudo dnf install --installroot=$(pwd)/Dev/Mount/Initrd --use-host-config $(cat Src/Extra.list)

    echo "${P}sudo dnf install -y --installroot=$(pwd)/Dev/Mount/Initrd Dev/Repo/*"
  sudo dnf install -y --installroot=$(pwd)/Dev/Mount/Initrd Dev/Repo/* >/dev/null 2>&1
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
