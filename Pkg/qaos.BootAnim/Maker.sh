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
GCC() {
  bash Tools/gcc.sh $*
}
moq() {
  bash Tools/moq.sh $*
}


# Const
P="  "
Self=$(dirname "$0")
Mach=$(Find "Architecture")


# Modes
build() {
  run GCC -shared "$Self/Dev/Main.cpp" -o "$Self/Lib/Main.so"



  # Compile Image
  case "$(crudini --get Dev/Device.conf Cover Theme)" in
  Black)
    TBack="#000000"
    TFore="#FFFFFF"
    ;;
  White)
    TBack="#FFFFFF"
    TFore="#000000"
    ;;
  esac

  sed 's/\$Back/'$TBack'/g; s/\$Fore/'$TFore'/g; s/\$Color/#'$(crudini --get Dev/Device.conf Cover Color)'/g' $Self/Dev/Generic.svg > $Self/Res/Boot.svg
  

  magick -density 480 $Self/Res/Boot.svg -resize 480x480 $Self/Res/Boot_480.bmp
  


  # Create moq
  moq .n $(basename $Self)
  moq .a $(basename $Self)  "/" "$Self/Package.info"
  moq .a $(basename $Self)  "/" "$Self/Bin"
  moq .a $(basename $Self)  "/" "$Self/Lib"
  moq .a $(basename $Self)  "/" "$Self/Res"
  moq .p $(basename $Self)
  run moq .i $(basename $Self)


  Nucs="Dev/Mount/System/Conf/Nucleol.conf"

  ListC=$(crudini --get $Nucs Root ListC)
  ListC=$((ListC + 1))
  crudini --set $Nucs Root ListC $ListC
  
  run crudini --set $Nucs Root List_$ListC $(basename $Self)

  crudini --set $Nucs $(basename $Self) Essential 1
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
