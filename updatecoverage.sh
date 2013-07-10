#!/usr/bin/env bash

# Updates the coverage documentation, and copies it into the appropriate place
# in the gh-pages branch.

set -e

tempdir=`mktemp -d`

make coverage COVERAGE_OUTPUT=${tempdir}/coverage

git checkout gh-pages

cp -r ${tempdir}/coverage* assets/coverage/

rm -rf ${tempdir}
