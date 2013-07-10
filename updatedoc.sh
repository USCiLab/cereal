#!/usr/bin/env bash

# Updates the doxygen documentation, and copies it into the appropriate place
# in the gh-pages branch.

set -e

tempdir=`mktemp -d`

make coverage COVERAGE_OUTPUT=${tempdir}/coverage

make doc

cp -r ./doc/html/ ${tempdir}

git checkout gh-pages

cp -r ${tempdir}/coverage* assets/coverage/

cp -r ${tempdir}/html/* assets/doxygen/

rm -rf ${tempdir}
