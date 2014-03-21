#!/usr/bin/env bash

destdir="include_renamed"

echo -n "New prefix: "
read newprefix

cp -r include ${destdir}

newprefix=$(echo ${newprefix} | sed -e 's/\//\\\//')

find ${destdir} -name '*.hpp' -exec sed -i "s/#include <cereal/#include <${newprefix}\/cereal/" {} \;
