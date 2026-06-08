#!/bin/bash

# Check the number of arguments
if [ $# -lt 1 ] || [ $# -gt 5 ]; then
  echo "Error: Please provide one to five arguments."
  return 1
fi

current_script_path=$(pwd)

if [ "$current_script_path" = "$HPM_SDK_BASE" ]; then
  export HPM_BOARD_SEARCH_PATH="$HPM_SDK_BASE/boards"
else
  HPM_BOARD_SEARCH_PATH_OPTIONS=()
  HPM_BOARD_SEARCH_PATH_OPTIONS+=("$current_script_path/boards")
  HPM_BOARD_SEARCH_PATH_OPTIONS+=("$HPM_SDK_BASE/boards")

  # Set the select prompt
  PS3="> "

  # Prompt the user to select an option for HPM_BOARD_SEARCH_PATH
  echo -e "\033[32mSelect an option for HPM_BOARD_SEARCH_PATH:\033[0m"
  select HPM_BOARD_SEARCH_PATH_OPTION in "${HPM_BOARD_SEARCH_PATH_OPTIONS[@]}"; do
    if [[ -n "$HPM_BOARD_SEARCH_PATH_OPTION" ]]; then
      export HPM_BOARD_SEARCH_PATH="$HPM_BOARD_SEARCH_PATH_OPTION"
      break
    else
      echo "Invalid option. Please select a valid option."
    fi
  done
fi

# Export the first argument as an environment variable
export HPM_DEMO=$1

# Export the second argument as an environment variable (if provided)
if [ $# -ge 2 ]; then
  export HPM_BOARD=$2
else

  # Find directories in the current directory starting with "hpm" and use them as options
  HPM_BOARD_OPTIONS=()
  while IFS= read -r -d '' dir; do
    HPM_BOARD_OPTIONS+=("$(basename "$dir")")
  done < <(find "$HPM_BOARD_SEARCH_PATH" -maxdepth 1 -type d -name "hpm*" -execdir printf '%s\0' {} +)

  # Sort the options
  HPM_BOARD_OPTIONS=($(printf '%s\n' "${HPM_BOARD_OPTIONS[@]}" | sort))

  # Set the select prompt
  PS3="> "

  # Prompt the user to select an option for HPM_BOARD
  echo -e "\033[32mSelect an option for HPM_BOARD:\033[0m"
  select HPM_BOARD_OPTION in "${HPM_BOARD_OPTIONS[@]}"; do
    if [[ -n "$HPM_BOARD_OPTION" ]]; then
      export HPM_BOARD="$HPM_BOARD_OPTION"
      break
    else
      echo "Invalid option. Please select a valid option."
    fi
  done

fi

# Export the third argument as an environment variable (if provided)
if [ $# -ge 3 ]; then
  export HPM_CMAKE_BUILD_TYPE=$3
else

  HPM_BUILD_OPTIONS=("ram" "flash_sdram_uf2" "flash_sdram_xip" "flash_uf2" "flash_xip")

  # Check if app.yaml file exists
  if [ -f "$HPM_DEMO/app.yaml" ]; then
    # Read the excluded_targets list from app.yaml file
    excluded_targets=$(awk '/excluded_targets:/ {flag=1; next} /^\w+:$/ {flag=0} flag && /^ *-/ {print substr($0, 3)}' $HPM_DEMO/app.yaml)
    excluded_targets=$(echo "$excluded_targets" | tr -d ' ')
    excluded_targets=$(echo "$excluded_targets" | tr -d '\n')
    excluded_targets=${excluded_targets#-}
    IFS='-' read -r excluded_targets <<<"$excluded_targets"

    HPM_BUILD_OPTIONS_EXCLUDED=()

    for element in "${HPM_BUILD_OPTIONS[@]}"; do
      found=0

      for item in "${excluded_targets[@]}"; do
        if [[ "$element" == "$item" ]]; then
          found=1
          break
        fi
      done

      if [[ "$found" -eq 0 ]]; then
        HPM_BUILD_OPTIONS_EXCLUDED+=("$element")
      fi
    done

  else

    HPM_BUILD_OPTIONS_EXCLUDED=()

    for element in "${HPM_BUILD_OPTIONS[@]}"; do
      HPM_BUILD_OPTIONS_EXCLUDED+=("$element")
    done

  fi

  # Sort the options
  HPM_BUILD_OPTIONS_EXCLUDED=($(printf '%s\n' "${HPM_BUILD_OPTIONS_EXCLUDED[@]}" | sort))

  # Set the select prompt
  PS3="> "

  # Prompt the user to select an option for HPM_CMAKE_BUILD_TYPE
  echo -e "\033[32mSelect an option for HPM_CMAKE_BUILD_TYPE:\033[0m"
  select HPM_BUILD_OPTION in "${HPM_BUILD_OPTIONS_EXCLUDED[@]}"; do
    if [[ -n "$HPM_BUILD_OPTION" ]]; then
      export HPM_CMAKE_BUILD_TYPE="$HPM_BUILD_OPTION"
      break
    else
      echo "Invalid option. Please select a valid option."
    fi
  done

fi

# Export the fouth argument as an environment variable (if provided)
if [ $# -ge 4 ]; then
  export HPM_PROBE=$4
else

  # Find config in the current directory and use them as options
  HPM_PROBE_OPTIONS=()

  while IFS= read -r -d '' file; do
    file_name=$(basename "$file")
    file_name="${file_name%.*}"
    HPM_PROBE_OPTIONS+=("$file_name")
  done < <(find "$HPM_BOARD_SEARCH_PATH/openocd/probes" -type f -execdir printf '%s\0' {} +)

  # Sort the options
  HPM_PROBE_OPTIONS=($(printf '%s\n' "${HPM_PROBE_OPTIONS[@]}" | sort))

  # Set the select prompt
  PS3="> "

  # Prompt the user to select an option for HPM_PROBE
  echo -e "\033[32mSelect an option for HPM_PROBE:\033[0m"
  select HPM_PROBE_OPTION in "${HPM_PROBE_OPTIONS[@]}"; do
    if [[ -n "$HPM_PROBE_OPTION" ]]; then
      export HPM_PROBE="$HPM_PROBE_OPTION"
      break
    else
      echo "Invalid option. Please select a valid option."
    fi
  done
fi

# Export the fifth argument as an environment variable (if provided)
if [ $# -ge 4 ]; then
  export HPM_SOC=$4
else

  # Find config in the current directory and use them as options
  HPM_SOC_ALL_OPTIONS=()

  while IFS= read -r -d '' file; do
    file_name=$(basename "$file")
    file_name="${file_name%.*}"
    HPM_SOC_ALL_OPTIONS+=("$file_name")
  done < <(find "$HPM_BOARD_SEARCH_PATH/openocd/soc" -type f -execdir printf '%s\0' {} +)

  cmp_prefix="${HPM_BOARD:0:5}"

  HPM_SOC_OPTIONS=()

  for item in "${HPM_SOC_ALL_OPTIONS[@]}"; do
    item_prefix="${item:0:5}"
    if [ "$item_prefix" = "$cmp_prefix" ]; then
      HPM_SOC_OPTIONS+=("$item")
    fi
  done

  # Sort the options
  HPM_SOC_OPTIONS=($(printf '%s\n' "${HPM_SOC_OPTIONS[@]}" | sort))

  # Set the select prompt
  PS3="> "

  # Prompt the user to select an option for HPM_SOC
  echo -e "\033[32mSelect an option for HPM_SOC:\033[0m"
  select HPM_SOC_OPTION in "${HPM_SOC_OPTIONS[@]}"; do
    if [[ -n "$HPM_SOC_OPTION" ]]; then
      export HPM_SOC="$HPM_SOC_OPTION"
      break
    else
      echo "Invalid option. Please select a valid option."
    fi
  done
fi

# Echo environment variable
echo -e "\033[32mThe compilation configuration is set to\033[0m"
echo "HPM_DEMO              : $HPM_DEMO"
echo "HPM_BOARD_SEARCH_PATH : $HPM_BOARD_SEARCH_PATH"
echo "HPM_BOARD             : $HPM_BOARD"
echo "HPM_CMAKE_BUILD_TYPE  : $HPM_CMAKE_BUILD_TYPE"
echo "HPM_PROBE             : $HPM_PROBE"
echo "HPM_SOC               : $HPM_SOC"
