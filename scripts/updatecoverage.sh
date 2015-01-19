#!/usr/bin/env bash

# Updates the coverage documentation, and copies it into the appropriate place
# in the gh-pages-develop branch.
#   $1 from CMAKE will contain the root directory of cereal

# this requires lcov 1.10 or newer

set -e

COVERAGE_TESTS=./coverage_*

# run tests
for f in $COVERAGE_TESTS
  do
    echo $f
    $f
  done

# build coverage output
tempdir=`mktemp -d`

lcov --capture --directory $1 --output-file coverage.info --no-external
lcov --remove coverage.info '*/external/*' 'cereal/details/util.hpp' 'sandbox/*' 'unittests/*' -o coverage.info
genhtml --demangle-cpp coverage.info --output-directory ${tempdir}

# copy over to gh pages
git checkout gh-pages-develop

rm -rf $1/assets/coverage
mkdir $1/assets/coverage
cp -r ${tempdir}/* $1/assets/coverage/
rm -rf ${tempdir}
