
# Modes
New() {
  # Clear
  rm -rf "Temp/$1"

  # New
  mkdir "Temp/$1"

}

Add() {
  mkdir -p "Temp/$1/$2"

  cp $3 "Temp/$1/$2"
}

Pack() {
  rm -f "Temp/$1".moq

  echo -n $(cd "Temp/$1"; zip -0qr ../"$1".moq * )
}

Install() {
  cp "Temp/$1".moq  "Mount/System/Moq"
}


# Router
case "$1" in
  .n|.new)
    New $2
    ;;
  .a|.add)
    Add $2 $3 $4
    ;;
  .p|.pack)
    Pack $2
    ;;
  .i|.install)
    Install $2
    ;;
  *)
    echo "Unknown Command"
    ;;
esac
