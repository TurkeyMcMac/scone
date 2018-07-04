set -ex

source=https://raw.githubusercontent.com/TurkeyMcMac/scone/master
tempdir=.scone-install

mkdir $tempdir
cd $tempdir

curl $source/scone.c  > scone.c
curl $source/scone.h  > scone.h
curl $source/version  > version
curl $source/Makefile > Makefile

sudo make install

cd ..
rm -rf $tempdir
