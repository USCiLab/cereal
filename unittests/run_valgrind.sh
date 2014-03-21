#!/usr/bin/env bash

set -e

TESTS=./test_*

for f in $TESTS
  do
    valgrind --tool=memcheck --leak-check=full $f
  done
