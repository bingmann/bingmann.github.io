#!/bin/sh -x

# etch/oldstable

apt-ftparchive generate etch.conf || exit

apt-ftparchive -c etch.conf release dists/etch > dists/etch/Release
apt-ftparchive -c etch.conf release dists/etch | gzip -9 > dists/etch/Release.gz
apt-ftparchive -c etch.conf release dists/etch | bzip2 -9 > dists/etch/Release.bz2

gpg -abs -u F4EF04FA -o- dists/etch/Release > dists/etch/Release.gpg

cp -avr dists/etch/Release dists/oldstable/Release
cp -avr dists/etch/Release.gz dists/oldstable/Release.gz
cp -avr dists/etch/Release.bz2 dists/oldstable/Release.bz2
cp -avr dists/etch/Release.gpg dists/oldstable/Release.gpg

# lenny/stable

apt-ftparchive generate lenny.conf || exit

apt-ftparchive -c lenny.conf release dists/lenny > dists/lenny/Release
apt-ftparchive -c lenny.conf release dists/lenny | gzip -9 > dists/lenny/Release.gz
apt-ftparchive -c lenny.conf release dists/lenny | bzip2 -9 > dists/lenny/Release.bz2

gpg -abs -u F4EF04FA -o- dists/lenny/Release > dists/lenny/Release.gpg

cp -avr dists/lenny/Release dists/stable/Release
cp -avr dists/lenny/Release.gz dists/stable/Release.gz
cp -avr dists/lenny/Release.bz2 dists/stable/Release.bz2
cp -avr dists/lenny/Release.gpg dists/stable/Release.gpg

# squeeze/testing

apt-ftparchive generate squeeze.conf || exit

apt-ftparchive -c squeeze.conf release dists/squeeze > dists/squeeze/Release
apt-ftparchive -c squeeze.conf release dists/squeeze | gzip -9 > dists/squeeze/Release.gz
apt-ftparchive -c squeeze.conf release dists/squeeze | bzip2 -9 > dists/squeeze/Release.bz2

gpg -abs -u F4EF04FA -o- dists/squeeze/Release > dists/squeeze/Release.gpg

cp -avr dists/squeeze/Release dists/testing/Release
cp -avr dists/squeeze/Release.gz dists/testing/Release.gz
cp -avr dists/squeeze/Release.bz2 dists/testing/Release.bz2
cp -avr dists/squeeze/Release.gpg dists/testing/Release.gpg
