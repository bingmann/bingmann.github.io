#!/bin/sh -x

for dist in stable oldstable; do

    mkdir -p dists/$dist
    mkdir -p dists/$dist/main/binary-amd64
    mkdir -p dists/$dist/main/binary-i386
    mkdir -p dists/$dist/main/source

done

for dist in etch lenny wheezy jessie; do

    mkdir -p dists/$dist
    mkdir -p dists/$dist/main/binary-amd64
    mkdir -p dists/$dist/main/binary-i386
    mkdir -p dists/$dist/main/source

    apt-ftparchive generate $dist.conf || exit

    apt-ftparchive -c $dist.conf release dists/$dist > dists/$dist/Release
    apt-ftparchive -c $dist.conf release dists/$dist | gzip -9 > dists/$dist/Release.gz
    apt-ftparchive -c $dist.conf release dists/$dist | bzip2 -9 > dists/$dist/Release.bz2

    gpg -abs -u F4EF04FA -o- dists/$dist/Release > dists/$dist/Release.gpg

done

cp -avr dists/wheezy/Release dists/oldstable/Release
cp -avr dists/wheezy/Release.gz dists/oldstable/Release.gz
cp -avr dists/wheezy/Release.bz2 dists/oldstable/Release.bz2
cp -avr dists/wheezy/Release.gpg dists/oldstable/Release.gpg

cp -avr dists/jessie/Release dists/stable/Release
cp -avr dists/jessie/Release.gz dists/stable/Release.gz
cp -avr dists/jessie/Release.bz2 dists/stable/Release.bz2
cp -avr dists/jessie/Release.gpg dists/stable/Release.gpg
