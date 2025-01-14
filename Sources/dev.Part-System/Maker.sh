# Def
set -e
set -o pipefail


# Const
Part="System"
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
  echo "${P}# Creating disk" 
  # Create Disk
  DiskSize=$(cat Config/DiskTable.tbl | grep "$Part" | cut -d' ' -f2)
  run truncate -s $DiskSize "Images/$Part.img"
  run mkfs.ext4 -q "Images/$Part.img"
  
  # Push Loop
  CLoop=$(sudo losetup -f)
  echo "${CLoop}" >> "Temp/Loop.list"
  
  # Mount Disk
  run sudo losetup ${CLoop} "Images/$Part.img"
  run mkdir "$WDir"
  run sudo mount ${CLoop} "$WDir"
  run sudo chown -R $(whoami) "$WDir"


  echo
  echo "${P}# Basic Amethyst"
    echo "${P}mkdir {System}/{Packages,Conf}"
  mkdir "$WDir"/{Packages,Conf}
}


# Router
case "$1" in
  build|*)
    build
    ;;
esac
