#!/usr/bin/env gnuplot

set terminal pdf dashed linewidth 1.5 size 5.0, 7.07
set output "distrospeed.pdf"

set xrange [10:1600000]
set format x "%.0f"

### Plot ###

set title "libgcrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Serpent" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Twofish" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Serpent" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Twofish" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Serpent" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Twofish" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Serpent" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Twofish" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Serpent" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Twofish" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Serpent" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Twofish" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "libmcrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Serpent" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Twofish" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST6" with errorlines pt 0 lt 1 lc 7, \
     "p4-3200-gentoo/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo GOST" with errorlines pt 0 lt 1 lc 8, \
     "p4-3200-gentoo/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Serpent" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Twofish" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST6" with errorlines pt 0 lt 2 lc 7, \
     "p4-3200-debian-etch/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch GOST" with errorlines pt 0 lt 2 lc 8, \
     "p4-3200-debian-etch/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Serpent" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Twofish" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST6" with errorlines pt 0 lt 4 lc 7, \
     "p4-3200-debian-lenny/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny GOST" with errorlines pt 0 lt 4 lc 8, \
     "p4-3200-debian-lenny/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Serpent" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Twofish" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST6" with errorlines pt 0 lt 5 lc 7, \
     "p4-3200-ubuntu-gutsy/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy GOST" with errorlines pt 0 lt 5 lc 8, \
     "p4-3200-ubuntu-gutsy/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Serpent" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Twofish" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST6" with errorlines pt 0 lt 6 lc 7, \
     "p4-3200-ubuntu-hardy/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy GOST" with errorlines pt 0 lt 6 lc 8, \
     "p4-3200-ubuntu-hardy/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Serpent" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Twofish" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST6" with errorlines pt 0 lt 8 lc 7, \
     "p4-3200-fedora8/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 GOST" with errorlines pt 0 lt 8 lc 8, \
     "p4-3200-fedora8/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "Botan Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Serpent" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Twofish" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST6" with errorlines pt 0 lt 1 lc 7, \
     "p4-3200-gentoo/botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo GOST" with errorlines pt 0 lt 1 lc 8, \
     "p4-3200-gentoo/botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo xTEA" with errorlines pt 0 lt 1 lc 9, \
     "p4-3200-gentoo/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Serpent" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Twofish" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST6" with errorlines pt 0 lt 2 lc 7, \
     "p4-3200-debian-etch/botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch GOST" with errorlines pt 0 lt 2 lc 8, \
     "p4-3200-debian-etch/botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch xTEA" with errorlines pt 0 lt 2 lc 9, \
     "p4-3200-debian-etch/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Serpent" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Twofish" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST6" with errorlines pt 0 lt 4 lc 7, \
     "p4-3200-debian-lenny/botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny GOST" with errorlines pt 0 lt 4 lc 8, \
     "p4-3200-debian-lenny/botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny xTEA" with errorlines pt 0 lt 4 lc 9, \
     "p4-3200-debian-lenny/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Serpent" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Twofish" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST6" with errorlines pt 0 lt 5 lc 7, \
     "p4-3200-ubuntu-gutsy/botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy GOST" with errorlines pt 0 lt 5 lc 8, \
     "p4-3200-ubuntu-gutsy/botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy xTEA" with errorlines pt 0 lt 5 lc 9, \
     "p4-3200-ubuntu-gutsy/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Serpent" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Twofish" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST6" with errorlines pt 0 lt 6 lc 7, \
     "p4-3200-ubuntu-hardy/botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy GOST" with errorlines pt 0 lt 6 lc 8, \
     "p4-3200-ubuntu-hardy/botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy xTEA" with errorlines pt 0 lt 6 lc 9, \
     "p4-3200-ubuntu-hardy/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Serpent" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Twofish" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST6" with errorlines pt 0 lt 8 lc 7, \
     "p4-3200-fedora8/botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 GOST" with errorlines pt 0 lt 8 lc 8, \
     "p4-3200-fedora8/botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 xTEA" with errorlines pt 0 lt 8 lc 9, \
     "p4-3200-fedora8/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "Crypto++ Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Serpent" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Twofish" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST6" with errorlines pt 0 lt 1 lc 7, \
     "p4-3200-gentoo/cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo GOST" with errorlines pt 0 lt 1 lc 8, \
     "p4-3200-gentoo/cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo xTEA" with errorlines pt 0 lt 1 lc 9, \
     "p4-3200-gentoo/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Serpent" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Twofish" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST6" with errorlines pt 0 lt 2 lc 7, \
     "p4-3200-debian-etch/cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch GOST" with errorlines pt 0 lt 2 lc 8, \
     "p4-3200-debian-etch/cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch xTEA" with errorlines pt 0 lt 2 lc 9, \
     "p4-3200-debian-etch/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Serpent" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Twofish" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST6" with errorlines pt 0 lt 4 lc 7, \
     "p4-3200-debian-lenny/cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny GOST" with errorlines pt 0 lt 4 lc 8, \
     "p4-3200-debian-lenny/cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny xTEA" with errorlines pt 0 lt 4 lc 9, \
     "p4-3200-debian-lenny/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Serpent" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Twofish" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST6" with errorlines pt 0 lt 5 lc 7, \
     "p4-3200-ubuntu-gutsy/cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy GOST" with errorlines pt 0 lt 5 lc 8, \
     "p4-3200-ubuntu-gutsy/cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy xTEA" with errorlines pt 0 lt 5 lc 9, \
     "p4-3200-ubuntu-gutsy/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Serpent" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Twofish" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST6" with errorlines pt 0 lt 6 lc 7, \
     "p4-3200-ubuntu-hardy/cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy GOST" with errorlines pt 0 lt 6 lc 8, \
     "p4-3200-ubuntu-hardy/cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy xTEA" with errorlines pt 0 lt 6 lc 9, \
     "p4-3200-ubuntu-hardy/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Serpent" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Twofish" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST6" with errorlines pt 0 lt 8 lc 7, \
     "p4-3200-fedora8/cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 GOST" with errorlines pt 0 lt 8 lc 8, \
     "p4-3200-fedora8/cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 xTEA" with errorlines pt 0 lt 8 lc 9, \
     "p4-3200-fedora8/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "OpenSSL Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "Nettle Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Serpent" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Twofish" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Serpent" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Twofish" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Serpent" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Twofish" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Serpent" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Twofish" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Serpent" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Twofish" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Serpent" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Twofish" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "Tomcrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Twofish" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Safer+" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Noekeon" with errorlines pt 0 lt 1 lc 12, \
     "p4-3200-gentoo/tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Skipjack" with errorlines pt 0 lt 1 lc 11, \
     "p4-3200-gentoo/tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Anubis" with errorlines pt 0 lt 1 lc 10, \
     "p4-3200-gentoo/tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo xTEA" with errorlines pt 0 lt 1 lc 9, \
     "p4-3200-gentoo/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo CAST5" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo 3DES" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Twofish" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Safer+" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Noekeon" with errorlines pt 0 lt 2 lc 12, \
     "p4-3200-debian-etch/tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Skipjack" with errorlines pt 0 lt 2 lc 11, \
     "p4-3200-debian-etch/tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Anubis" with errorlines pt 0 lt 2 lc 10, \
     "p4-3200-debian-etch/tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch xTEA" with errorlines pt 0 lt 2 lc 9, \
     "p4-3200-debian-etch/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch CAST5" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch 3DES" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Twofish" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Safer+" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Noekeon" with errorlines pt 0 lt 4 lc 12, \
     "p4-3200-debian-lenny/tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Skipjack" with errorlines pt 0 lt 4 lc 11, \
     "p4-3200-debian-lenny/tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Anubis" with errorlines pt 0 lt 4 lc 10, \
     "p4-3200-debian-lenny/tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny xTEA" with errorlines pt 0 lt 4 lc 9, \
     "p4-3200-debian-lenny/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny CAST5" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny 3DES" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Twofish" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Safer+" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Noekeon" with errorlines pt 0 lt 5 lc 12, \
     "p4-3200-ubuntu-gutsy/tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Skipjack" with errorlines pt 0 lt 5 lc 11, \
     "p4-3200-ubuntu-gutsy/tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Anubis" with errorlines pt 0 lt 5 lc 10, \
     "p4-3200-ubuntu-gutsy/tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy xTEA" with errorlines pt 0 lt 5 lc 9, \
     "p4-3200-ubuntu-gutsy/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy CAST5" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy 3DES" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Twofish" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Safer+" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Noekeon" with errorlines pt 0 lt 6 lc 12, \
     "p4-3200-ubuntu-hardy/tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Skipjack" with errorlines pt 0 lt 6 lc 11, \
     "p4-3200-ubuntu-hardy/tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Anubis" with errorlines pt 0 lt 6 lc 10, \
     "p4-3200-ubuntu-hardy/tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy xTEA" with errorlines pt 0 lt 6 lc 9, \
     "p4-3200-ubuntu-hardy/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy CAST5" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy 3DES" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Twofish" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Safer+" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Noekeon" with errorlines pt 0 lt 8 lc 12, \
     "p4-3200-fedora8/tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Skipjack" with errorlines pt 0 lt 8 lc 11, \
     "p4-3200-fedora8/tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Anubis" with errorlines pt 0 lt 8 lc 10, \
     "p4-3200-fedora8/tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 xTEA" with errorlines pt 0 lt 8 lc 9, \
     "p4-3200-fedora8/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 CAST5" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 3DES" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "Beecrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Rijndael" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Blowfish" with errorlines pt 0 lt 1 lc 4, \
\
     "p4-3200-debian-etch/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Rijndael" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Blowfish" with errorlines pt 0 lt 2 lc 4, \
\
     "p4-3200-debian-lenny/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Rijndael" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Blowfish" with errorlines pt 0 lt 4 lc 4, \
\
     "p4-3200-ubuntu-gutsy/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Rijndael" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Blowfish" with errorlines pt 0 lt 5 lc 4, \
\
     "p4-3200-ubuntu-hardy/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Rijndael" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Blowfish" with errorlines pt 0 lt 6 lc 4, \
\
     "p4-3200-fedora8/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Rijndael" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Blowfish" with errorlines pt 0 lt 8 lc 4

### Plot ###

set title "Rijndael AES: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libgcrypt" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libmcrypt" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Botan" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Crypto++" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo OpenSSL" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Nettle" with errorlines pt 0 lt 1 lc 6, \
     "p4-3200-gentoo/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Beecrypt" with errorlines pt 0 lt 1 lc 7, \
     "p4-3200-gentoo/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Tomcrypt" with errorlines pt 0 lt 1 lc 8, \
     "p4-3200-gentoo/my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Custom" with errorlines pt 0 lt 1 lc 9, \
\
     "p4-3200-debian-etch/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libgcrypt" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libmcrypt" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Botan" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Crypto++" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch OpenSSL" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Nettle" with errorlines pt 0 lt 2 lc 6, \
     "p4-3200-debian-etch/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Beecrypt" with errorlines pt 0 lt 2 lc 7, \
     "p4-3200-debian-etch/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Tomcrypt" with errorlines pt 0 lt 2 lc 8, \
     "p4-3200-debian-etch/my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Custom" with errorlines pt 0 lt 2 lc 9, \
\
     "p4-3200-debian-lenny/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libgcrypt" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libmcrypt" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Botan" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Crypto++" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny OpenSSL" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Nettle" with errorlines pt 0 lt 4 lc 6, \
     "p4-3200-debian-lenny/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Beecrypt" with errorlines pt 0 lt 4 lc 7, \
     "p4-3200-debian-lenny/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Tomcrypt" with errorlines pt 0 lt 4 lc 8, \
     "p4-3200-debian-lenny/my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Custom" with errorlines pt 0 lt 4 lc 9, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libgcrypt" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libmcrypt" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Botan" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Crypto++" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy OpenSSL" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Nettle" with errorlines pt 0 lt 5 lc 6, \
     "p4-3200-ubuntu-gutsy/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Beecrypt" with errorlines pt 0 lt 5 lc 7, \
     "p4-3200-ubuntu-gutsy/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Tomcrypt" with errorlines pt 0 lt 5 lc 8, \
     "p4-3200-ubuntu-gutsy/my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Custom" with errorlines pt 0 lt 5 lc 9, \
\
     "p4-3200-ubuntu-hardy/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libgcrypt" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libmcrypt" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Botan" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Crypto++" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy OpenSSL" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Nettle" with errorlines pt 0 lt 6 lc 6, \
     "p4-3200-ubuntu-hardy/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Beecrypt" with errorlines pt 0 lt 6 lc 7, \
     "p4-3200-ubuntu-hardy/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Tomcrypt" with errorlines pt 0 lt 6 lc 8, \
     "p4-3200-ubuntu-hardy/my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Custom" with errorlines pt 0 lt 6 lc 9, \
\
     "p4-3200-fedora8/gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libgcrypt" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libmcrypt" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Botan" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Crypto++" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 OpenSSL" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Nettle" with errorlines pt 0 lt 8 lc 6, \
     "p4-3200-fedora8/beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Beecrypt" with errorlines pt 0 lt 8 lc 7, \
     "p4-3200-fedora8/tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Tomcrypt" with errorlines pt 0 lt 8 lc 8, \
     "p4-3200-fedora8/my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Custom" with errorlines pt 0 lt 8 lc 9

### Plot ###

set title "Serpent: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libgcrypt" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libmcrypt" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Botan" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Crypto++" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Nettle" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Gladman" with errorlines pt 0 lt 1 lc 6, \
     "p4-3200-gentoo/mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo MyBotan" with errorlines pt 0 lt 1 lc 7, \
\
     "p4-3200-debian-etch/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libgcrypt" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libmcrypt" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Botan" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Crypto++" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Nettle" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Gladman" with errorlines pt 0 lt 2 lc 6, \
     "p4-3200-debian-etch/mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch MyBotan" with errorlines pt 0 lt 2 lc 7, \
\
     "p4-3200-debian-lenny/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libgcrypt" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libmcrypt" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Botan" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Crypto++" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Nettle" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Gladman" with errorlines pt 0 lt 4 lc 6, \
     "p4-3200-debian-lenny/mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny MyBotan" with errorlines pt 0 lt 4 lc 7, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libgcrypt" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libmcrypt" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Botan" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Crypto++" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Nettle" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Gladman" with errorlines pt 0 lt 5 lc 6, \
     "p4-3200-ubuntu-gutsy/mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy MyBotan" with errorlines pt 0 lt 5 lc 7, \
\
     "p4-3200-ubuntu-hardy/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libgcrypt" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libmcrypt" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Botan" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Crypto++" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Nettle" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Gladman" with errorlines pt 0 lt 6 lc 6, \
     "p4-3200-ubuntu-hardy/mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy MyBotan" with errorlines pt 0 lt 6 lc 7, \
\
     "p4-3200-fedora8/gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libgcrypt" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libmcrypt" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Botan" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Crypto++" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Nettle" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Gladman" with errorlines pt 0 lt 8 lc 6, \
     "p4-3200-fedora8/mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 MyBotan" with errorlines pt 0 lt 8 lc 7

### Plot ###

set title "Twofish: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libgcrypt" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libmcrypt" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Botan" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Crypto++" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Nettle" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Tomcrypt" with errorlines pt 0 lt 1 lc 6, \
\
     "p4-3200-debian-etch/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libgcrypt" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libmcrypt" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Botan" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Crypto++" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Nettle" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Tomcrypt" with errorlines pt 0 lt 2 lc 6, \
\
     "p4-3200-debian-lenny/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libgcrypt" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libmcrypt" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Botan" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Crypto++" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Nettle" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Tomcrypt" with errorlines pt 0 lt 4 lc 6, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libgcrypt" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libmcrypt" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Botan" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Crypto++" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Nettle" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Tomcrypt" with errorlines pt 0 lt 5 lc 6, \
\
     "p4-3200-ubuntu-hardy/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libgcrypt" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libmcrypt" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Botan" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Crypto++" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Nettle" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Tomcrypt" with errorlines pt 0 lt 6 lc 6, \
\
     "p4-3200-fedora8/gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libgcrypt" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libmcrypt" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Botan" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Crypto++" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Nettle" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Tomcrypt" with errorlines pt 0 lt 8 lc 6

### Plot ###

set title "Blowfish: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libgcrypt" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libmcrypt" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Botan" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Crypto++" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo OpenSSL" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Nettle" with errorlines pt 0 lt 1 lc 6, \
     "p4-3200-gentoo/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Beecrypt" with errorlines pt 0 lt 1 lc 7, \
     "p4-3200-gentoo/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Tomcrypt" with errorlines pt 0 lt 1 lc 8, \
\
     "p4-3200-debian-etch/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libgcrypt" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libmcrypt" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Botan" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Crypto++" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch OpenSSL" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Nettle" with errorlines pt 0 lt 2 lc 6, \
     "p4-3200-debian-etch/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Beecrypt" with errorlines pt 0 lt 2 lc 7, \
     "p4-3200-debian-etch/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Tomcrypt" with errorlines pt 0 lt 2 lc 8, \
\
     "p4-3200-debian-lenny/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libgcrypt" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libmcrypt" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Botan" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Crypto++" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny OpenSSL" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Nettle" with errorlines pt 0 lt 4 lc 6, \
     "p4-3200-debian-lenny/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Beecrypt" with errorlines pt 0 lt 4 lc 7, \
     "p4-3200-debian-lenny/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Tomcrypt" with errorlines pt 0 lt 4 lc 8, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libgcrypt" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libmcrypt" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Botan" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Crypto++" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy OpenSSL" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Nettle" with errorlines pt 0 lt 5 lc 6, \
     "p4-3200-ubuntu-gutsy/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Beecrypt" with errorlines pt 0 lt 5 lc 7, \
     "p4-3200-ubuntu-gutsy/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Tomcrypt" with errorlines pt 0 lt 5 lc 8, \
\
     "p4-3200-ubuntu-hardy/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libgcrypt" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libmcrypt" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Botan" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Crypto++" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy OpenSSL" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Nettle" with errorlines pt 0 lt 6 lc 6, \
     "p4-3200-ubuntu-hardy/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Beecrypt" with errorlines pt 0 lt 6 lc 7, \
     "p4-3200-ubuntu-hardy/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Tomcrypt" with errorlines pt 0 lt 6 lc 8, \
\
     "p4-3200-fedora8/gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libgcrypt" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libmcrypt" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Botan" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Crypto++" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 OpenSSL" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Nettle" with errorlines pt 0 lt 8 lc 6, \
     "p4-3200-fedora8/beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Beecrypt" with errorlines pt 0 lt 8 lc 7, \
     "p4-3200-fedora8/tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Tomcrypt" with errorlines pt 0 lt 8 lc 8

### Plot ###

set title "CAST5: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libgcrypt" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libmcrypt" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Botan" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Crypto++" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo OpenSSL" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Nettle" with errorlines pt 0 lt 1 lc 6, \
     "p4-3200-gentoo/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Tomcrypt" with errorlines pt 0 lt 1 lc 7, \
\
     "p4-3200-debian-etch/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libgcrypt" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libmcrypt" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Botan" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Crypto++" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch OpenSSL" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Nettle" with errorlines pt 0 lt 2 lc 6, \
     "p4-3200-debian-etch/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Tomcrypt" with errorlines pt 0 lt 2 lc 7, \
\
     "p4-3200-debian-lenny/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libgcrypt" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libmcrypt" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Botan" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Crypto++" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny OpenSSL" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Nettle" with errorlines pt 0 lt 4 lc 6, \
     "p4-3200-debian-lenny/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Tomcrypt" with errorlines pt 0 lt 4 lc 7, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libgcrypt" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libmcrypt" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Botan" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Crypto++" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy OpenSSL" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Nettle" with errorlines pt 0 lt 5 lc 6, \
     "p4-3200-ubuntu-gutsy/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Tomcrypt" with errorlines pt 0 lt 5 lc 7, \
\
     "p4-3200-ubuntu-hardy/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libgcrypt" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libmcrypt" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Botan" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Crypto++" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy OpenSSL" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Nettle" with errorlines pt 0 lt 6 lc 6, \
     "p4-3200-ubuntu-hardy/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Tomcrypt" with errorlines pt 0 lt 6 lc 7, \
\
     "p4-3200-fedora8/gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libgcrypt" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libmcrypt" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Botan" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Crypto++" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 OpenSSL" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Nettle" with errorlines pt 0 lt 8 lc 6, \
     "p4-3200-fedora8/tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Tomcrypt" with errorlines pt 0 lt 8 lc 7

### Plot ###

set title "Triple DES: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "p4-3200-gentoo/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libgcrypt" with errorlines pt 0 lt 1 lc 1, \
     "p4-3200-gentoo/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo libmcrypt" with errorlines pt 0 lt 1 lc 2, \
     "p4-3200-gentoo/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Botan" with errorlines pt 0 lt 1 lc 3, \
     "p4-3200-gentoo/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Crypto++" with errorlines pt 0 lt 1 lc 4, \
     "p4-3200-gentoo/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo OpenSSL" with errorlines pt 0 lt 1 lc 5, \
     "p4-3200-gentoo/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Nettle" with errorlines pt 0 lt 1 lc 6, \
     "p4-3200-gentoo/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gentoo Tomcrypt" with errorlines pt 0 lt 1 lc 7, \
\
     "p4-3200-debian-etch/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libgcrypt" with errorlines pt 0 lt 2 lc 1, \
     "p4-3200-debian-etch/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch libmcrypt" with errorlines pt 0 lt 2 lc 2, \
     "p4-3200-debian-etch/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Botan" with errorlines pt 0 lt 2 lc 3, \
     "p4-3200-debian-etch/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Crypto++" with errorlines pt 0 lt 2 lc 4, \
     "p4-3200-debian-etch/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch OpenSSL" with errorlines pt 0 lt 2 lc 5, \
     "p4-3200-debian-etch/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Nettle" with errorlines pt 0 lt 2 lc 6, \
     "p4-3200-debian-etch/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Etch Tomcrypt" with errorlines pt 0 lt 2 lc 7, \
\
     "p4-3200-debian-lenny/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libgcrypt" with errorlines pt 0 lt 4 lc 1, \
     "p4-3200-debian-lenny/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny libmcrypt" with errorlines pt 0 lt 4 lc 2, \
     "p4-3200-debian-lenny/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Botan" with errorlines pt 0 lt 4 lc 3, \
     "p4-3200-debian-lenny/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Crypto++" with errorlines pt 0 lt 4 lc 4, \
     "p4-3200-debian-lenny/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny OpenSSL" with errorlines pt 0 lt 4 lc 5, \
     "p4-3200-debian-lenny/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Nettle" with errorlines pt 0 lt 4 lc 6, \
     "p4-3200-debian-lenny/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Debian-Lenny Tomcrypt" with errorlines pt 0 lt 4 lc 7, \
\
     "p4-3200-ubuntu-gutsy/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libgcrypt" with errorlines pt 0 lt 5 lc 1, \
     "p4-3200-ubuntu-gutsy/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy libmcrypt" with errorlines pt 0 lt 5 lc 2, \
     "p4-3200-ubuntu-gutsy/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Botan" with errorlines pt 0 lt 5 lc 3, \
     "p4-3200-ubuntu-gutsy/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Crypto++" with errorlines pt 0 lt 5 lc 4, \
     "p4-3200-ubuntu-gutsy/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy OpenSSL" with errorlines pt 0 lt 5 lc 5, \
     "p4-3200-ubuntu-gutsy/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Nettle" with errorlines pt 0 lt 5 lc 6, \
     "p4-3200-ubuntu-gutsy/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Gutsy Tomcrypt" with errorlines pt 0 lt 5 lc 7, \
\
     "p4-3200-ubuntu-hardy/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libgcrypt" with errorlines pt 0 lt 6 lc 1, \
     "p4-3200-ubuntu-hardy/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy libmcrypt" with errorlines pt 0 lt 6 lc 2, \
     "p4-3200-ubuntu-hardy/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Botan" with errorlines pt 0 lt 6 lc 3, \
     "p4-3200-ubuntu-hardy/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Crypto++" with errorlines pt 0 lt 6 lc 4, \
     "p4-3200-ubuntu-hardy/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy OpenSSL" with errorlines pt 0 lt 6 lc 5, \
     "p4-3200-ubuntu-hardy/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Nettle" with errorlines pt 0 lt 6 lc 6, \
     "p4-3200-ubuntu-hardy/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Ubuntu-Hardy Tomcrypt" with errorlines pt 0 lt 6 lc 7, \
\
     "p4-3200-fedora8/gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libgcrypt" with errorlines pt 0 lt 8 lc 1, \
     "p4-3200-fedora8/mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 libmcrypt" with errorlines pt 0 lt 8 lc 2, \
     "p4-3200-fedora8/botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Botan" with errorlines pt 0 lt 8 lc 3, \
     "p4-3200-fedora8/cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Crypto++" with errorlines pt 0 lt 8 lc 4, \
     "p4-3200-fedora8/openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 OpenSSL" with errorlines pt 0 lt 8 lc 5, \
     "p4-3200-fedora8/nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Nettle" with errorlines pt 0 lt 8 lc 6, \
     "p4-3200-fedora8/tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Fedora 8 Tomcrypt" with errorlines pt 0 lt 8 lc 7
