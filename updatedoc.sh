#!/usr/bin/env bash

# Updates the doxygen documentation, and copies it into the appropriate place
# in the gh-pages branch.

make doc

tempdir=`mktemp -d`

cp -r ./doc/html/ ${tempdir}

git checkout gh-pages

cp -r ${tempdir}/html/* assets/doxygen/

rm -rf ${tempdir}
