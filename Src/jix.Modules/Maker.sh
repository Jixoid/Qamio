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
  run sudo cp -r /lib/modules/$(Find Kernel_Name) Dev/Mount/Initrd/lib/modules 
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
