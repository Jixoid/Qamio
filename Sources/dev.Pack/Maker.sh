# Def
set -e
set -o pipefail


# Const
P="  "


# Lib
run() {
  echo "${P}$*"
  "$@"
}


# Modes
build() {
  echo "${P}# Syncing images"
  run sync

  # Images
  echo "${P}# Compressing images"
  ls ./Images | while read file; do
    run xz -k "Images/$file"
    echo -n $(sha256sum "Images/$file" | cut -d' ' -f1) >> "Images/$file°sha256"
  done
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
