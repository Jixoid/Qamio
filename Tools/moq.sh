
# Modes
New() {
  # Clear
  rm -rf "Dev/Temp/$1"

  # New
  mkdir "Dev/Temp/$1"

}

Add() {
  mkdir -p "Dev/Temp/$1/$2"

  cp -r $3 "Dev/Temp/$1/$2"
}

Pack() {
  rm -f "Dev/Temp/$1".moq

  echo -n $(cd "Dev/Temp/$1"; tar -cf ../"$1".moq * )
}

Install() {
  cp "Dev/Temp/$1".moq  "Dev/Mount/System/Moq"
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
