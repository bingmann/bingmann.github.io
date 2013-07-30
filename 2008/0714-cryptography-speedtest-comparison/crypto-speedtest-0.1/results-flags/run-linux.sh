#!/bin/bash -x

for f in [ig]cc*; do
    sync
    pushd $f
    [ -e my-rijndael-ecb.txt ] || ./speedtest_custom
    popd
done

for f in [ig]cc*; do
    pushd $f
    [ -e cryptopp-rijndael-ecb.txt ] || ./speedtest_cryptopp
    popd
done
