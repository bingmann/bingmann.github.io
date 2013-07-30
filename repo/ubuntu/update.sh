#!/bin/sh -x

eval $(gpg-agent --daemon)

for dist in hardy karmic lucid maverick natty oneiric precise; do

    apt-ftparchive generate $dist.conf || exit

    apt-ftparchive -c $dist.conf release dists/$dist > dists/$dist/Release
    apt-ftparchive -c $dist.conf release dists/$dist | gzip -9 > dists/$dist/Release.gz
    apt-ftparchive -c $dist.conf release dists/$dist | bzip2 -9 > dists/$dist/Release.bz2
    
    gpg -abs -u F4EF04FA -o- dists/$dist/Release > dists/$dist/Release.gpg

done
