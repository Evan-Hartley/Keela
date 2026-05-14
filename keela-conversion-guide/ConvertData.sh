#!/bin/bash
    
if [[ "$#" < 4 ]]; then
    echo ' '
    echo "This script requires 4 arguments:"
    echo '  root-path-to-recording-files'
    echo '  YYYYMMDD'
    echo '  number-of-cameras' 
    echo '  number-of-bits'
    echo ' '
    exit -1
fi

set -euo pipefail

# Paths and parameters
path_to_files="${1}"
date=${2}
cam_num=${3}
bit_num=${4}


file_type=".mkv"

for cam in {1..cam_num}; do
    # Name conventions
    name_convention="${date}_*_cam_${cam_num}"
    file_convention="${path_to_files}/${name_convention}${file_type}"

    # Get sorted file list (numeric sort based on digits in filename)
    file_list=$(ls $file_convention 2>/dev/null | sort -V)

    # Prepare output list
    input_list=()

    for full_str in $file_list; do
        # Normalize slashes
        full_str="${full_str//\\//}"
        
        python ConvertData-${bit-num}bit.py "$full_str"

    done
done
