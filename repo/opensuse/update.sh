#!/bin/sh -x

for ver in 11.2 11.3 11.4 12.1; do

    rm -rf $ver/repodata/

    yum-createrepo $ver/

    cp -a ../key.asc $ver/repodata/repomd.xml.key
    gpg -abs -u F4EF04FA $ver/repodata/repomd.xml

done
