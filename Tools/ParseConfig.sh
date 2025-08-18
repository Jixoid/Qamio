
# Reset
echo -n "" > def.list

# Parse
while IFS='=' read -r Key Val; do
  Key=$(echo "$Key" | tr -d '[:space:]')
  Val=$(echo "$Val" | tr -d '[:space:]')

  # Dump
  if [ "$Val" = "y" ]; then
    echo -n "$Key " >> def.list
  fi

done < ".config"
