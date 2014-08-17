#!/bin/bash
#
# File:  test.sh
# Author:  mikolas
# Created on:  Tue Aug 5 20:46:39 WEST 2014
# Copyright (C) 2014, Mikolas Janota
#
for f in ./examples/err*.qcir; do
  echo "$f ==============="
  ./qcir_sort $f
  if [ $? -eq 0 ]; then
    echo '===============FAIL'
    exit 100
  fi
done
for f in ./examples/t*.qcir; do
  echo "$f ==============="
  ./qcir_sort $f
  if [ $? -ne 0 ]; then
    echo '===============FAIL'
    exit 100
  fi
done
echo '===============OK'
exit 0
