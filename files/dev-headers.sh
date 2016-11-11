#!/usr/bin/env sh
set -xe

# dpkg-deb -c ./qtcreator-dev_4.0.2-1ubuntu2\~1_all.deb | perl -pe 's/^.+\s+([^\s]+)$/$1/'

D=$OPAMSHARE/qtcreator-dev
rm -fr $D
scripts/createDevPackage.py $D -s . -b . -v

#find . -name '*.pri' -exec rsync -R -v {} $D \;
