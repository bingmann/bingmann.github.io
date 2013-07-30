#!/bin/sh -x

rm -rf 9/repodata/

yum-createrepo 9/

cp -a ../key.asc 9/repodata/repomd.xml.key
gpg -abs -u F4EF04FA 9/repodata/repomd.xml


rm -rf 10/repodata/

yum-createrepo 10/

cp -a ../key.asc 10/repodata/repomd.xml.key
gpg -abs -u F4EF04FA 10/repodata/repomd.xml


rm -rf 11/repodata/

yum-createrepo 11/

cp -a ../key.asc 11/repodata/repomd.xml.key
gpg -abs -u F4EF04FA 11/repodata/repomd.xml


rm -rf 12/repodata/

yum-createrepo 12/

cp -a ../key.asc 12/repodata/repomd.xml.key
gpg -abs -u F4EF04FA 12/repodata/repomd.xml


rm -rf 13/repodata/

yum-createrepo 13/

cp -a ../key.asc 13/repodata/repomd.xml.key
gpg -abs -u F4EF04FA 13/repodata/repomd.xml


rm -rf 14/repodata/

yum-createrepo 14/

cp -a ../key.asc 14/repodata/repomd.xml.key
gpg -abs -u F4EF04FA 14/repodata/repomd.xml
