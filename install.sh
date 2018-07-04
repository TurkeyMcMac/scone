set -ex

source=https://raw.githubusercontent.com/TurkeyMcMac/scone/master
tempdir=.scone-install

mkdir $tempdir
cd $tempdir

_download() { curl $source/$1  > $1 }

_download scone.c
_download scone.h
_download version
_download Makefile

sudo make install

cd ..
rm -rf $tempdir
