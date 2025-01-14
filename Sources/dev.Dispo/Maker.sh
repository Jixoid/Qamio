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
  echo "${P}# Unmount files" 
  ls ./Mount | while read file; do
    run sudo umount Mount/"$file"
    run sudo rm -r Mount/"$file"
  done

  echo
  echo "${P}# Dispose loops"
  if [ -f "Temp/Loop.list" ]; then
    run sudo losetup -d $(cat "Temp/Loop.list")
    run rm "Temp/Loop.list"
  fi
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
