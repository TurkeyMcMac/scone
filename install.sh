set -ex

source=https://raw.githubusercontent.com/TurkeyMcMac/scone/master
tempdir=`mktemp -d`
_download() {
	curl -s $source/$1  > $1
}

cd $tempdir

_download version
if [ ! -f /usr/lib/libscone.so.`cat version` ]; then
	_download scone.c &
	_download scone.h &
	_download Makefile &
	wait
	sudo make install
fi

cd ..
rm -rf $tempdir
