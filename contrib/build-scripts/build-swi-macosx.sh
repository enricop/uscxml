#!/bin/bash

#
# build SWI Prolog for MacOSX
#

# exit on error
set -e

ME=`basename $0`
DIR="$( cd "$( dirname "$0" )" && pwd )" 
MACOSX_VER=`/usr/bin/sw_vers -productVersion`
MACOSX_COMP=(`echo $MACOSX_VER | tr '.' ' '`)
DEST_DIR="${DIR}/../prebuilt/darwin-i386/${MACOSX_COMP[0]}.${MACOSX_COMP[1]}/gnu"
VERSION=`cat VERSION`

if [ ! -f src/pl-main.c ]; then
	echo
	echo "Cannot find src/pl-main.c"
	echo "Run script from within SWI prolog directory:"
	echo "pl-devel$ ../../${ME}"
	echo
	exit
fi

./prepare
cd src
if [ -f Makefile ]; then
	make clean
fi

if [ ${MACOSX_COMP[1]} -lt 9 ]; then
  export CXXFLAGS="-g -mmacosx-version-min=10.6 -stdlib=libstdc++ -arch x86_64 -arch i386"
  export CFLAGS="-g -mmacosx-version-min=10.6 -arch x86_64 -arch i386"
  export LDFLAGS="-stdlib=libstdc++"
else
  export CXXFLAGS="-g -mmacosx-version-min=10.7 -stdlib=libc++ -arch x86_64 -arch i386"
  export CFLAGS="-g -mmacosx-version-min=10.7 -stdlib=libc++ -arch x86_64 -arch i386"
  export LDFLAGS="-stdlib=libc++"
fi

#CPPFLAGS="-DHAVE_CURSES_H=0 -DHAVE_TGETENT=0 -DHAVE_TCSETATTR=0 -DHAVE_TERM_H=0 -DHAVE_LIBNCURSES=0" \

./configure \
--disable-gmp --disable-readline \
--without-jpl \
--prefix=${DEST_DIR}

sed -ie 's/define HAVE_CURSES_H 1/undef HAVE_CURSES_H/' config.h
sed -ie 's/define HAVE_TGETENT 1/undef HAVE_TGETENT/' config.h
sed -ie 's/define HAVE_TCSETATTR 1/undef HAVE_TCSETATTR/' config.h
sed -ie 's/define HAVE_TERM_H 1/undef HAVE_TERM_H/' config.h
sed -ie 's/define HAVE_LIBNCURSES 1/undef HAVE_LIBNCURSES/' config.h

make -j2
make install
make clean

cd ../packages/cpp
# ./configure --prefix=${DEST_DIR}
# make install

cp SWI-cpp.h ${DEST_DIR}/lib/swipl-${VERSION}/include

# export PATH=$PATH:${DEST_DIR}/lib/swipl-6.3.5/bin/x86_64-darwin12.2.0/


rm -rf ${DEST_DIR}/bin
rm -rf ${DEST_DIR}/share
rm -rf ${DEST_DIR}/lib/pkgconfig