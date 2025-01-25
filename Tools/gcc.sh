# Def
set -e
set -o pipefail


# Lib
Find () {
  echo "$(cat Conf/Compile.conf | grep -w "$1" | cut -d' ' -f2)"
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
"-I./Inc" "-I./Nucs" $PicC "-std=$(Find GCC_Std)" -lc -lstdc++ "-O$(Find Optimized)" $* \
$(cat Conf/Definition.conf | grep -E 'Yes' | sed 's/: Yes$//' | sed 's/^/-D/' | tr '\n' ' ' )
