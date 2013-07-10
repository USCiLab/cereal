#!/usr/bin/env bash

# Updates the doxygen documentation, and copies it into the appropriate place
# in the gh-pages branch.

set -e

tempdir=`mktemp -d`

make doc

cp -r ./doc/html/ ${tempdir}

git checkout gh-pages

rm -rf assets/doxygen
mkdir assets/doxygen
cp -r ${tempdir}/html/* assets/doxygen/

rm -rf ${tempdir}
