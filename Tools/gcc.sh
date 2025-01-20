# Def
set -e
set -o pipefail


# Lib
Find () {
  echo "$(cat Config/Compile.conf | grep "$1" | cut -d' ' -f2)"
}


# Main
case "$(Find "Architecture")" in
  x64)
    Comp="gcc"
    ;;
  a64)
    Comp="aarch64-linux-gnu-gcc -I/usr/arm-none-eabi/include/"
    ;;
esac

PicC=""
if [[ $(Find "PicCode") == "Yes" ]] then
  PicC="-fPIC"
fi


$Comp \
"-I./Interfaces" $PicC "-std=$(Find GCC_Std)" -lc -lstdc++ "-O$(Find Optimized)" $* \
$(cat Config/Definition.conf | grep -E 'Yes' | sed 's/: Yes$//' | sed 's/^/-D/' | tr '\n' ' ' )
