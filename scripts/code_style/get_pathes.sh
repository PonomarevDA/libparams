#!/bin/bash
CODE_STYLE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPTS_DIR="$(dirname "$CODE_STYLE_DIR")"
REPO_DIR="$(dirname "$SCRIPTS_DIR")"

FOLDERS=(
   "$REPO_DIR/libparams/*"
   "$REPO_DIR/platform_specific/*/*"
)

source_files=()
file_counter=0
for file in ${FOLDERS[@]}; do
   if [ -f "$file" ]; then
      if [[ $file == *.c ]] || [[ $file == *.h ]] || [[ $file == *.cpp ]] || [[ $file == *.hpp ]] ; then
         source_files+=($file)
         let "file_counter=file_counter+1"
      fi
   fi
done

for file in ${source_files[@]}; do
   echo $file
done
