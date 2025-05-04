Config=".config"

Defs=""


while IFS='=' read -r Key Val; do
  Key=$(echo "$Key" | tr -d '[:space:]')
  Val=$(echo "$Val" | tr -d '[:space:]')

  if [ "$Val" = "y" ]; then
    Defs+="-D$Key "
  fi
done < "$Config"

echo $Defs
