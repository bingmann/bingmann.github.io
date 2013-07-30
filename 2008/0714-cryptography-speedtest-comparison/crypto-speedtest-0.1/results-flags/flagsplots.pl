#!/usr/bin/perl

my @plots =
(
 [ "my-rijndael-ecb", "Custom Rijndael" ],
 [ "gladman-serpent-ecb", "Gladman Serpent" ],
 [ "mybotan-serpent-ecb", "MyBotan Serpent" ],
 [ "cryptopp-rijndael-ecb", "Crypto++ Rijndael" ],
 [ "cryptopp-serpent-ecb", "Crypto++ Serpent" ],
 [ "cryptopp-twofish-ecb", "Crypto++ Twofish" ],
 [ "cryptopp-cast6-ecb", "Crypto++ CAST6" ],
 [ "cryptopp-gost-ecb", "Crypto++ GOST" ],
 [ "cryptopp-xtea-ecb", "Crypto++ XTEA" ],
 [ "cryptopp-blowfish-ecb", "Crypto++ Blowfish" ],
 [ "cryptopp-cast5-ecb", "Crypto++ CAST5" ],
 [ "cryptopp-3des-ecb", "Crypto++ 3DES" ],
);

sub gnuplot_flagsgcc34
{
    print <<EOF;
#!/usr/bin/env gnuplot

set terminal pdf dashed size 5.0, 5.07
set output 'flags-gcc3.4.pdf'

set xrange [10:1600000]
set format x "%.0f"
EOF

    for my $p (@plots)
    {
	print <<EOF;
### Plot ###

set title "$$p[1] - Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot \\
     "gcc34-O0/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O0" with errorlines pt 0 lt 1 lc 1, \\
     "gcc34-O1/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O1" with errorlines pt 0 lt 1 lc 2, \\
     "gcc34-O2/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O2" with errorlines pt 0 lt 1 lc 3, \\
     "gcc34-O3/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3" with errorlines pt 0 lt 1 lc 4, \\
     "gcc34-Os/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -Os" with errorlines pt 0 lt 1 lc 5, \\
     "gcc34-O2-p4/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O2 p4" with errorlines pt 0 lt 1 lc 6, \\
     "gcc34-O2-p4-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O2 p4 ofp" with errorlines pt 0 lt 1 lc 7, \\
     "gcc34-O2-p4s-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O2 p4s ofp" with errorlines pt 0 lt 1 lc 8, \\
     "gcc34-O3-p4/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4" with errorlines pt 0 lt 1 lc 9, \\
     "gcc34-O3-p4-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4 ofp" with errorlines pt 0 lt 1 lc 10, \\
     "gcc34-O3-p4s-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4s ofp" with errorlines pt 0 lt 1 lc 11, \\
     "gcc34-O3-p4s-ofp-ul/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4s ofp ul" with errorlines pt 0 lt 1 lc 12, \\
\\
     "mingw-O0/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "MinGW -O0" with errorlines pt 0 lt 4 lc 1, \\
     "mingw-O1/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "MinGW -O1" with errorlines pt 0 lt 4 lc 2, \\
     "mingw-O2/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "MinGW -O2" with errorlines pt 0 lt 4 lc 3, \\
     "mingw-O3/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "MinGW -O3" with errorlines pt 0 lt 4 lc 4, \\
     "mingw-Os/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "MinGW -Os" with errorlines pt 0 lt 4 lc 5 \\

EOF
    }
}

sub gnuplot_flags
{
    print <<EOF;
#!/usr/bin/env gnuplot

set terminal pdf dashed size 5.0, 5.07
set output 'flags.pdf'

set xrange [10:1600000]
set format x "%.0f"
EOF

    for my $p (@plots)
    {
	print <<EOF;
### Plot ###

set title "$$p[1] - Speed by Data Length"
set xlabel "Data Length in Bytes"
set ylabel "Megabyte / Second"
set logscale x
unset logscale y
set key below

plot \\
     "gcc34-O0/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O0" with errorlines pt 0 lt 2 lc 1, \\
     "gcc34-O1/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O1" with errorlines pt 0 lt 2 lc 2, \\
     "gcc34-O2/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O2" with errorlines pt 0 lt 2 lc 3, \\
     "gcc34-O3/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3" with errorlines pt 0 lt 2 lc 4, \\
     "gcc34-Os/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -Os" with errorlines pt 0 lt 2 lc 5, \\
     "gcc34-O3-p4/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4" with errorlines pt 0 lt 2 lc 9, \\
     "gcc34-O3-p4-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4 ofp" with errorlines pt 0 lt 2 lc 10, \\
     "gcc34-O3-p4s-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4s ofp" with errorlines pt 0 lt 2 lc 11, \\
     "gcc34-O3-p4s-ofp-ul/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 3.4.6 -O3 p4s ofp ul" with errorlines pt 0 lt 2 lc 12, \\
\\
     "gcc41-O0/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O0" with errorlines pt 0 lt 1 lc 1, \\
     "gcc41-O1/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O1" with errorlines pt 0 lt 1 lc 2, \\
     "gcc41-O2/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O2" with errorlines pt 0 lt 1 lc 3, \\
     "gcc41-O3/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O3" with errorlines pt 0 lt 1 lc 4, \\
     "gcc41-Os/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -Os" with errorlines pt 0 lt 1 lc 5, \\
     "gcc41-O3-p4/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O3 p4" with errorlines pt 0 lt 1 lc 6, \\
     "gcc41-O3-p4-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O3 p4 ofp" with errorlines pt 0 lt 1 lc 7, \\
     "gcc41-O3-p4s-ofp/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O3 p4s ofp" with errorlines pt 0 lt 1 lc 8, \\
     "gcc41-O3-p4s-ofp-ul/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "gcc 4.1.2 -O3 p4s ofp ul" with errorlines pt 0 lt 1 lc 9, \\
\\
     "icc-O0/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "icc 10.0 -O0" with errorlines pt 0 lt 4 lc 1, \\
     "icc-O1/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "icc 10.0 -O1" with errorlines pt 0 lt 4 lc 2, \\
     "icc-O2/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "icc 10.0 -O2" with errorlines pt 0 lt 4 lc 3, \\
     "icc-O3/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "icc 10.0 -O3" with errorlines pt 0 lt 4 lc 4, \\
     "icc-Os/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "icc 10.0 -Os" with errorlines pt 0 lt 4 lc 5, \\
\\
     "msvc8-Od/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "msvc8 -Od" with errorlines pt 0 lt 6 lc 1, \\
     "msvc8-O1/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "msvc8 -O1" with errorlines pt 0 lt 6 lc 2, \\
     "msvc8-O2/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "msvc8 -O2" with errorlines pt 0 lt 6 lc 3, \\
     "msvc8-Ox/$$p[0].txt" index 1 using 1:(\$2 / 1048576):(\$3 / 1048576) title "msvc8 -Ox" with errorlines pt 0 lt 6 lc 4

EOF
    }
}

open(GP, "| gnuplot") or die("Could not create a piped gnuplot.");
select(GP);
gnuplot_flagsgcc34();
close(GP);

open(GP, "| gnuplot") or die("Could not create a piped gnuplot.");
select(GP);
gnuplot_flags();
close(GP);
