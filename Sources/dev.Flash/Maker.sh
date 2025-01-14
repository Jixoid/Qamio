# Def
set -e
set -o pipefail


# Const
P="  "


# Variables
WDev="/dev/sda"


# Lib
run() {
  echo "${P}$*"
  "$@"
}


# Modes
build() {
  echo -n "${P}Device path: $WDev"
  #read WDev
  echo

  # Disk reset
  run sudo dd if=/dev/zero of="$WDev" bs=2048 count=1 conv=notrunc status=none

  run sudo parted "$WDev" mklabel gpt -s

  # Parts
  run sudo parted "$WDev" mkpart primary 2048s 1230847s -s
  run sudo parted "$WDev" mkpart primary 1230848s 9615359s -s
  run sudo parted "$WDev" mkpart primary 9615360s 60086271s -s

}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
