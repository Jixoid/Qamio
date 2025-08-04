
# Reset
echo -n "" > config.h

# Parse
while IFS='=' read -r Key Val; do
  Key=$(echo "$Key" | tr -d '[:space:]')
  Val=$(echo "$Val" | tr -d '[:space:]')

  # Dump
  if [ "$Val" = "y" ]; then
    echo "#define $Key" >> config.h
  fi

done < ".config"
