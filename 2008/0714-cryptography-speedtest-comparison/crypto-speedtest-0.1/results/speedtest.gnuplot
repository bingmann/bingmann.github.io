#!/usr/bin/env gnuplot

# Magic to get the different plot formats and output file names from the
# environment variable set by the Makefile

set macros
name = "'`echo $PLOTNAME`'"

cd @name

filebase = "'`echo $FILENAME`'"
if (@filebase eq '') filebase = "@name"

# for the A4-plots: width / height = sqrt(2)
paper = "'`echo $PAPER`'"
if (@paper eq 'a4') \
    set terminal pdf solid linewidth 1.5 size 4.0, 2.8328; \
else \
    set terminal pdf solid linewidth 1.5 size 5.0, 3.5;

set output "../" . @filebase . ".pdf"

set xrange [10:1600000]
set format x "%.0f"
set label @name at screen 0.0,1.0 offset 2.5,-1.5

### Plot ###

set title "libgcrypt Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "gcrypt-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "gcrypt-serpent-ecb.txt" index 0 using 1:2:3 title "Serpent" with errorlines pt 0, \
     "gcrypt-twofish-ecb.txt" index 0 using 1:2:3 title "Twofish" with errorlines pt 0, \
     "gcrypt-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "gcrypt-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "gcrypt-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "libgcrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Serpent" with errorlines pt 0, \
     "gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0, \
     "gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "libmcrypt Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "mcrypt-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "mcrypt-serpent-ecb.txt" index 0 using 1:2:3 title "Serpent" with errorlines pt 0, \
     "mcrypt-twofish-ecb.txt" index 0 using 1:2:3 title "Twofish" with errorlines pt 0, \
     "mcrypt-cast6-ecb.txt" index 0 using 1:2:3 title "CAST6" with errorlines pt 0, \
     "mcrypt-xtea-ecb.txt" index 0 using 1:2:3 title "xTEA" with errorlines pt 0, \
     "mcrypt-saferplus-ecb.txt" index 0 using 1:2:3 title "Safer+" with errorlines pt 0, \
     "mcrypt-loki97-ecb.txt" index 0 using 1:2:3 title "Loki97" with errorlines pt 0, \
     "mcrypt-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "mcrypt-gost-ecb.txt" index 0 using 1:2:3 title "GOST" with errorlines pt 0, \
     "mcrypt-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "mcrypt-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "libmcrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Serpent" with errorlines pt 0, \
     "mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0, \
     "mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST6" with errorlines pt 0, \
     "mcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "xTEA" with errorlines pt 0, \
     "mcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Safer+" with errorlines pt 0, \
     "mcrypt-loki97-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Loki97" with errorlines pt 0, \
     "mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "GOST" with errorlines pt 0, \
     "mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "Botan Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "botan-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "botan-serpent-ecb.txt" index 0 using 1:2:3 title "Serpent" with errorlines pt 0, \
     "botan-twofish-ecb.txt" index 0 using 1:2:3 title "Twofish" with errorlines pt 0, \
     "botan-cast6-ecb.txt" index 0 using 1:2:3 title "CAST6" with errorlines pt 0, \
     "botan-gost-ecb.txt" index 0 using 1:2:3 title "GOST" with errorlines pt 0, \
     "botan-xtea-ecb.txt" index 0 using 1:2:3 title "xTEA" with errorlines pt 0, \
     "botan-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "botan-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "botan-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "Botan Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Serpent" with errorlines pt 0, \
     "botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0, \
     "botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST6" with errorlines pt 0, \
     "botan-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "GOST" with errorlines pt 0, \
     "botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "xTEA" with errorlines pt 0, \
     "botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "Crypto++ Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "cryptopp-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "cryptopp-serpent-ecb.txt" index 0 using 1:2:3 title "Serpent" with errorlines pt 0, \
     "cryptopp-twofish-ecb.txt" index 0 using 1:2:3 title "Twofish" with errorlines pt 0, \
     "cryptopp-cast6-ecb.txt" index 0 using 1:2:3 title "CAST6" with errorlines pt 0, \
     "cryptopp-gost-ecb.txt" index 0 using 1:2:3 title "GOST" with errorlines pt 0, \
     "cryptopp-xtea-ecb.txt" index 0 using 1:2:3 title "xTEA" with errorlines pt 0, \
     "cryptopp-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "cryptopp-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "cryptopp-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "Crypto++ Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Serpent" with errorlines pt 0, \
     "cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0, \
     "cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST6" with errorlines pt 0, \
     "cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "GOST" with errorlines pt 0, \
     "cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "xTEA" with errorlines pt 0, \
     "cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "Crypto++ Ciphers: Speed by Data Length (2)"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0 lc 1, \
     "cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Serpent" with errorlines pt 0 lc 2, \
     "cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0 lc 3, \
     "cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST6" with errorlines pt 0 lc 4, \
     "cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "GOST" with errorlines pt 0 lc 5, \
     "cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "xTEA" with errorlines pt 0 lc 6, \
     "cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0 lc 8, \
     "cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0 lc 9

### Plot ###

set title "OpenSSL Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "openssl-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "openssl-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "openssl-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "openssl-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "OpenSSL Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "Nettle Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "nettle-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "nettle-serpent-ecb.txt" index 0 using 1:2:3 title "Serpent" with errorlines pt 0, \
     "nettle-twofish-ecb.txt" index 0 using 1:2:3 title "Twofish" with errorlines pt 0, \
     "nettle-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "nettle-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "nettle-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "Nettle Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Serpent" with errorlines pt 0, \
     "nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0, \
     "nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "Tomcrypt Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "tomcrypt-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "tomcrypt-twofish-ecb.txt" index 0 using 1:2:3 title "Twofish" with errorlines pt 0, \
     "tomcrypt-saferplus-ecb.txt" index 0 using 1:2:3 title "Safer+" with errorlines pt 0, \
     "tomcrypt-noekeon-ecb.txt" index 0 using 1:2:3 title "Noekeon" with errorlines pt 0, \
     "tomcrypt-skipjack-ecb.txt" index 0 using 1:2:3 title "Skipjack" with errorlines pt 0, \
     "tomcrypt-khazad-ecb.txt" index 0 using 1:2:3 title "Khazad" with errorlines pt 0, \
     "tomcrypt-anubis-ecb.txt" index 0 using 1:2:3 title "Anubis" with errorlines pt 0, \
     "tomcrypt-xtea-ecb.txt" index 0 using 1:2:3 title "xTEA" with errorlines pt 0, \
     "tomcrypt-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0, \
     "tomcrypt-cast5-ecb.txt" index 0 using 1:2:3 title "CAST5" with errorlines pt 0, \
     "tomcrypt-3des-ecb.txt" index 0 using 1:2:3 title "3DES" with errorlines pt 0

### Plot ###

set title "Tomcrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Twofish" with errorlines pt 0, \
     "tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Safer+" with errorlines pt 0, \
     "tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Noekeon" with errorlines pt 0, \
     "tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Skipjack" with errorlines pt 0, \
     "tomcrypt-khazad-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Khazad" with errorlines pt 0, \
     "tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Anubis" with errorlines pt 0, \
     "tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "xTEA" with errorlines pt 0, \
     "tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0, \
     "tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "CAST5" with errorlines pt 0, \
     "tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "3DES" with errorlines pt 0

### Plot ###

set title "Beecrypt Ciphers: Absolute Time by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Seconds"
set logscale x
set logscale y
set key below

plot "beecrypt-rijndael-ecb.txt" index 0 using 1:2:3 title "Rijndael" with errorlines pt 0, \
     "beecrypt-blowfish-ecb.txt" index 0 using 1:2:3 title "Blowfish" with errorlines pt 0

### Plot ###

set title "Beecrypt Ciphers: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Rijndael" with errorlines pt 0, \
     "beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Blowfish" with errorlines pt 0

### Plot ###

set title "Rijndael AES: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libgcrypt" with errorlines pt 0, \
     "mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libmcrypt" with errorlines pt 0, \
     "botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Botan" with errorlines pt 0, \
     "cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Crypto++" with errorlines pt 0, \
     "openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "OpenSSL" with errorlines pt 0, \
     "nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Nettle" with errorlines pt 0, \
     "beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Beecrypt" with errorlines pt 0, \
     "tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Tomcrypt" with errorlines pt 0, \
     "my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Custom" with errorlines pt 0

### Plot ###

set title "Serpent: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libgcrypt" with errorlines pt 0, \
     "mcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libmcrypt" with errorlines pt 0, \
     "botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Botan" with errorlines pt 0, \
     "cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Crypto++" with errorlines pt 0, \
     "nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Nettle" with errorlines pt 0, \
     "gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Gladman" with errorlines pt 0, \
     "mybotan-serpent-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "MyBotan" with errorlines pt 0

### Plot ###

set title "Twofish: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libgcrypt" with errorlines pt 0, \
     "mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libmcrypt" with errorlines pt 0, \
     "botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Botan" with errorlines pt 0, \
     "cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Crypto++" with errorlines pt 0, \
     "nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Nettle" with errorlines pt 0, \
     "tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Tomcrypt" with errorlines pt 0

### Plot ###

set title "Blowfish: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libgcrypt" with errorlines pt 0, \
     "mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libmcrypt" with errorlines pt 0, \
     "botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Botan" with errorlines pt 0, \
     "cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Crypto++" with errorlines pt 0, \
     "openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "OpenSSL" with errorlines pt 0, \
     "nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Nettle" with errorlines pt 0, \
     "beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Beecrypt" with errorlines pt 0, \
     "tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Tomcrypt" with errorlines pt 0

### Plot ###

set title "CAST5: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libgcrypt" with errorlines pt 0, \
     "mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libmcrypt" with errorlines pt 0, \
     "botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Botan" with errorlines pt 0, \
     "cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Crypto++" with errorlines pt 0, \
     "openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "OpenSSL" with errorlines pt 0, \
     "nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Nettle" with errorlines pt 0, \
     "tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Tomcrypt" with errorlines pt 0

### Plot ###

set title "Triple DES: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot "gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libgcrypt" with errorlines pt 0, \
     "mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "libmcrypt" with errorlines pt 0, \
     "botan-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Botan" with errorlines pt 0, \
     "cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Crypto++" with errorlines pt 0, \
     "openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "OpenSSL" with errorlines pt 0, \
     "nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Nettle" with errorlines pt 0, \
     "tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576):($3 / 1048576) title "Tomcrypt" with errorlines pt 0

### Plot ###

set terminal pdf dashed linewidth 2.0 size 5.0, 7.07
set output "../" . @name . "-all.pdf"

set title "All Tests: Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot \
     "botan-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan 3DES" with lines lt 2 lc 4, \
     "botan-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan Blowfish" with lines lt 2 lc 5, \
     "botan-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan CAST5" with lines lt 2 lc 6, \
     "botan-cast6-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan CAST6" with lines lt 2 lc 7, \
     "botan-gost-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan GOST" with lines lt 2 lc 8, \
     "botan-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan Rijndael" with lines lt 2 lc 1, \
     "botan-serpent-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan Serpent" with lines lt 2 lc 2, \
     "botan-twofish-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan Twofish" with lines lt 2 lc 3, \
     "botan-xtea-ecb.txt" index 1 using 1:($2 / 1048576) title "Botan XTEA" with lines lt 2 lc 7, \
     "cryptopp-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ 3DES" with lines lt 3 lc 4, \
     "cryptopp-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ Blowfish" with lines lt 3 lc 5, \
     "cryptopp-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ CAST5" with lines lt 3 lc 6, \
     "cryptopp-cast6-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ CAST6" with lines lt 3 lc 7, \
     "cryptopp-gost-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ GOST" with lines lt 3 lc 8, \
     "cryptopp-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ Rijndael" with lines lt 3 lc 1, \
     "cryptopp-serpent-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ Serpent" with lines lt 3 lc 2, \
     "cryptopp-twofish-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ Twofish" with lines lt 3 lc 3, \
     "cryptopp-xtea-ecb.txt" index 1 using 1:($2 / 1048576) title "Crypto++ XTEA" with lines lt 3 lc 7, \
     "gcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "libgcrypt 3DES" with lines lt 4 lc 4, \
     "gcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "libgcrypt Blowfish" with lines lt 4 lc 5, \
     "gcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "libgcrypt CAST5" with lines lt 4 lc 6, \
     "gcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "libgcrypt Rijndael" with lines lt 4 lc 1, \
     "gcrypt-serpent-ecb.txt" index 1 using 1:($2 / 1048576) title "libgcrypt Serpent" with lines lt 4 lc 2, \
     "gcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576) title "libgcrypt Twofish" with lines lt 4 lc 3, \
     "mcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt 3DES" with lines lt 5 lc 4, \
     "mcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt Blowfish" with lines lt 5 lc 5, \
     "mcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt CAST5" with lines lt 5 lc 6, \
     "mcrypt-cast6-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt CAST6" with lines lt 5 lc 7, \
     "mcrypt-gost-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt GOST" with lines lt 5 lc 8, \
     "mcrypt-loki97-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt Loki97" with lines lt 5 lc 0, \
     "mcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt Rijndael" with lines lt 5 lc 1, \
     "mcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt Safer+" with lines lt 5 lc 10, \
     "mcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt Twofish" with lines lt 5 lc 3, \
     "mcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576) title "libmcrypt XTEA" with lines lt 5 lc 7, \
     "my-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "Custom Rijndael" with lines lt 1 lc 1, \
     "gladman-serpent-ecb.txt" index 1 using 1:($2 / 1048576) title "Gladman Serpent" with lines lt 1 lc 2, \
     "openssl-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "OpenSSL 3DES" with lines lt 7 lc 4, \
     "openssl-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "OpenSSL Blowfish" with lines lt 7 lc 5, \
     "openssl-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "OpenSSL CAST5" with lines lt 7 lc 6, \
     "openssl-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "OpenSSL Rijndael" with lines lt 7 lc 1, \
     "nettle-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "Nettle 3DES" with lines lt 8 lc 4, \
     "nettle-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "Nettle Blowfish" with lines lt 8 lc 5, \
     "nettle-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "Nettle CAST5" with lines lt 8 lc 6, \
     "nettle-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "Nettle Rijndael" with lines lt 8 lc 1, \
     "nettle-serpent-ecb.txt" index 1 using 1:($2 / 1048576) title "Nettle Serpent" with lines lt 8 lc 2, \
     "nettle-twofish-ecb.txt" index 1 using 1:($2 / 1048576) title "Nettle Twofish" with lines lt 8 lc 3, \
     "tomcrypt-3des-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt 3DES" with lines lt 6 lc 4, \
     "tomcrypt-anubis-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Anubis" with lines lt 6 lc 11, \
     "tomcrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Blowfish" with lines lt 6 lc 5, \
     "tomcrypt-cast5-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt CAST5" with lines lt 6 lc 6, \
     "tomcrypt-khazad-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Khazad" with lines lt 6 lc 12, \
     "tomcrypt-noekeon-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Noekeon" with lines lt 6 lc 20, \
     "tomcrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Rijndael" with lines lt 6 lc 1, \
     "tomcrypt-saferplus-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Safer+" with lines lt 6 lc 10, \
     "tomcrypt-skipjack-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Skipjack" with lines lt 6 lc 14, \
     "tomcrypt-twofish-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt Twofish" with lines lt 6 lc 3, \
     "tomcrypt-xtea-ecb.txt" index 1 using 1:($2 / 1048576) title "Tomcrypt XTEA" with lines lt 6 lc 7

#     "beecrypt-blowfish-ecb.txt" index 1 using 1:($2 / 1048576) title "Beecrypt Blowfish" with lines lt 6 lc 5, \
#     "beecrypt-rijndael-ecb.txt" index 1 using 1:($2 / 1048576) title "Beecrypt Rijndael" with lines lt 6 lc 1,
