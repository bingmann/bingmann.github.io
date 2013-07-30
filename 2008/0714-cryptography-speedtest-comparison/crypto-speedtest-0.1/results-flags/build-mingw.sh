#!/bin/sh -x

MYPWD=`pwd`

build_speedtest_custom()
{
    local SUBDIR=$1
    local CFLAGS=$2
    local CXXFLAGS=$2

    [ -e $SUBDIR ] || mkdir -p $SUBDIR

    $CXX -o $SUBDIR/speedtest_custom $CXXFLAGS -I../src \
	../src/speedtest_custom.cpp \
	../src/rijndael.cpp \
	../src/serpent-gladman.cpp \
	../src/serpent.cpp || exit
}

build_cryptopp()
{
    local SUBDIR=$1
    local CFLAGS="$2 -DNDEBUG -pipe"
    local CXXFLAGS="$2 -DNDEBUG -pipe"

    [ -e $SUBDIR ] || mkdir -p $SUBDIR
    rm -rf $SUBDIR/cryptopp

    rm -rf cryptopp-5.5.2
    mkdir cryptopp-5.5.2
    pushd cryptopp-5.5.2
    unzip ../cryptopp552.zip || exit

    echo "Building cryptopp-5.5.2 using cc $CC cflags $CFLAGS"

    touch nothing.exe
    make libcryptopp.a install \
	"CC=$CC" "CFLAGS=$CFLAGS" \
	"CXX=$CXX" "CXXFLAGS=$CXXFLAGS" \
	"PREFIX=$MYPWD/$SUBDIR/cryptopp" \
	|| exit

    popd
}

build_speedtest_cryptopp()
{
    local SUBDIR=$1
    local CFLAGS=$2
    local CXXFLAGS=$2

    if [ ! -e $SUBDIR/cryptopp ]; then
	build_cryptopp "$1" "$2"
    fi

    $CXX -o $SUBDIR/speedtest_cryptopp $CXXFLAGS \
	-DCRYPTOPP_INCLUDE_PREFIX=cryptopp \
	-I../src -I$SUBDIR/cryptopp/include \
	../src/speedtest_cryptopp.cpp \
	$SUBDIR/cryptopp/lib/libcryptopp.a || exit
}

build_speedtest()
{
    build_speedtest_custom "$1" "$2"
	build_speedtest_cryptopp "$1" "$2"
}

if [ ! -e "cryptopp552.zip" ]; then
    echo "Copy cryptopp552.zip into this directory."
    exit
fi

CC=gcc
CXX=g++

build_speedtest "mingw-O0" "-O0"
build_speedtest "mingw-O1" "-O1"
build_speedtest "mingw-O2" "-O2"
build_speedtest "mingw-O3" "-O3"
build_speedtest "mingw-Os" "-Os"
