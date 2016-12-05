#!/bin/sh
set -e

for i in configs/*; do
  case "${i}" in
    configs/config-* )      continue ;;
    configs/addconfig-64k ) continue ;;
  esac

  make CONFIG="$i"
done
