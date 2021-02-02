#! /bin/bash

function _compile() {
  local filename="${1##*/}"
  local object_file_path="dist/${filename%.asm}.o"
  nasm -f elf "$1" -o "$object_file_path" || return 1
  ld -m elf_i386 "$object_file_path" -o "dist/${filename%.asm}" || return 1
}

if [[ -z "$1" || ! -e "$1" ]]; then
  echo "Provide existing filepath as first argument"
  exit 1
fi

mkdir dist 2>/dev/null
_compile "$1"
