#!/bin/bash -x

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

    if [ ! -e "cryptopp552.zip" ]; then
        echo "Copy cryptopp552.zip into this directory."
        exit
    fi

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

build_libgcrypt()
{
    local SUBDIR=$1
    local CFLAGS=$2
    local CXXFLAGS=$2

    rm -rf libgcrypt-1.4.0
    tar xjf libgcrypt-1.4.0.tar.bz2 || exit

    [ -e $SUBDIR ] || mkdir -p $SUBDIR
    rm -rf $SUBDIR/libgcrypt

    echo "Building libgcrypt-1.4.0 using cc $CC cflags $CFLAGS"

    pushd libgcrypt-1.4.0
    CC=$CC CFLAGS=$CFLAGS \
    CXX=$CC CXXFLAGS=$CFLAGS \
    ./configure --prefix=$MYPWD/$SUBDIR/libgcrypt \
	--disable-dependency-tracking \
	--disable-padlock-support || exit

    make install || exit
    popd
}

build_speedtest_gcrypt()
{
    local SUBDIR=$1
    local CFLAGS=$2
    local CXXFLAGS=$2

    if [ ! -e $SUBDIR/libgcrypt ]; then
	build_libgcrypt "$1" "$2"
    fi

    $CXX -o $SUBDIR/speedtest_gcrypt $CXXFLAGS \
	-I../src -I$SUBDIR/libgcrypt/include \
	../src/speedtest_gcrypt.cpp \
	$SUBDIR/libgcrypt/lib/libgcrypt.a -lgpg-error
}

build_libtomcrypt()
{
    local SUBDIR=$1
    local CFLAGS=$2
    local CXXFLAGS=$2

    rm -rf libtomcrypt-1.17
    tar xjf crypt-1.17.tar.bz2 || exit

    [ -e $SUBDIR ] || mkdir -p $SUBDIR
    rm -rf $SUBDIR/libtomcrypt

    echo "Building libtomcrypt-1.17 using cc $CC cflags $CFLAGS"

    pushd libtomcrypt-1.17
    CC=$CC CFLAGS=$CFLAGS \
    IGNORE_SPEED=1 NODOCS=1 \
    DESTDIR=$MYPWD/$SUBDIR/libtomcrypt \
    INSTALL_USER=$USER \
    make install || exit

    popd
}

build_speedtest_tomcrypt()
{
    local SUBDIR=$1
    local CFLAGS=$2
    local CXXFLAGS=$2

    if [ ! -e $SUBDIR/libtomcrypt ]; then
	build_libtomcrypt "$1" "$2"
    fi

    $CXX -o $SUBDIR/speedtest_tomcrypt $CXXFLAGS \
	-I../src -I$SUBDIR/libtomcrypt/usr/include \
	../src/speedtest_tomcrypt.cpp \
	$SUBDIR/libtomcrypt/usr/lib/libtomcrypt.a || exit
}

build_speedtest()
{
    build_speedtest_custom "$1" "$2"
    build_speedtest_cryptopp "$1" "$2"
}

CC=gcc-3.4.6
CXX=g++-3.4.6

build_speedtest "gcc34-O0" "-O0"
build_speedtest "gcc34-O1" "-O1"
build_speedtest "gcc34-O2" "-O2"
build_speedtest "gcc34-O3" "-O3"
build_speedtest "gcc34-Os" "-Os"

build_speedtest "gcc34-O2-p4" "-O2 -march=pentium4"
build_speedtest "gcc34-O3-p4" "-O3 -march=pentium4"

build_speedtest "gcc34-O2-p4-ofp" "-O2 -march=pentium4 -fomit-frame-pointer"
build_speedtest "gcc34-O3-p4-ofp" "-O3 -march=pentium4 -fomit-frame-pointer"

build_speedtest "gcc34-O2-p4s-ofp" "-O2 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer"
build_speedtest "gcc34-O3-p4s-ofp" "-O3 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer"

build_speedtest "gcc34-O2-p4s-ofp-ul" "-O2 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer -funroll-loops"
build_speedtest "gcc34-O3-p4s-ofp-ul" "-O3 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer -funroll-loops"

CC=gcc-4.1.2
CXX=g++-4.1.2

build_speedtest "gcc41-O0" "-O0"
build_speedtest "gcc41-O1" "-O1"
build_speedtest "gcc41-O2" "-O2"
build_speedtest "gcc41-O3" "-O3"
build_speedtest "gcc41-Os" "-Os"

build_speedtest "gcc41-O2-p4" "-O2 -march=pentium4"
build_speedtest "gcc41-O3-p4" "-O3 -march=pentium4"

build_speedtest "gcc41-O2-p4-ofp" "-O2 -march=pentium4 -fomit-frame-pointer"
build_speedtest "gcc41-O3-p4-ofp" "-O3 -march=pentium4 -fomit-frame-pointer"

build_speedtest "gcc41-O2-p4s-ofp" "-O2 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer"
build_speedtest "gcc41-O3-p4s-ofp" "-O3 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer"

build_speedtest "gcc41-O2-p4s-ofp-ul" "-O2 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer -funroll-loops"
build_speedtest "gcc41-O3-p4s-ofp-ul" "-O3 -march=pentium4 -msse -msse2 -msse3 -mfpmath=sse -fomit-frame-pointer -funroll-loops"

CC=icc
CXX=icc

build_speedtest "icc-O0" "-O0"
build_speedtest "icc-O1" "-O1"
build_speedtest "icc-O2" "-O2"
build_speedtest "icc-O3" "-O3"
build_speedtest "icc-Os" "-Os"
