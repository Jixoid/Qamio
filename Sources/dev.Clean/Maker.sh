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
    echo "${P}rm -rf Images/*"
  rm -rf "Images/"*

    echo "${P}find Core -type f -exec rm {} \;"
  find Core -type f -exec rm {} \;

    echo "${P}rm -rf Logs/*"
  rm -rf "Logs/"*

    echo "${P}rm -rf Temp/*"
  rm -rf "Temp/"*
}


# Router
case "$1" in
    build|*)
        build
        ;;
esac
