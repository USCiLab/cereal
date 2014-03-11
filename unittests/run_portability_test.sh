#!/usr/bin/env bash

set -e

./portability_test64 save 64
./portability_test32 load 32
./portability_test32 save 32
./portability_test64 load 64
./portability_test64 remove 64
