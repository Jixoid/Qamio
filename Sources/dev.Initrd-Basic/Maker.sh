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
  run sudo chown -R alforce:alforce Mount/Initrd
  run sudo chmod -R 777 Mount/Initrd/

  run mkdir "$WDir"/new_root
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
