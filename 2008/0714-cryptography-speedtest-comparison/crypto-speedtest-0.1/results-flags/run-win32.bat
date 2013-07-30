echo Running speedtests

cd mingw-O0
speedtest_custom
speedtest_cryptopp
cd ..

cd mingw-O1
speedtest_custom
speedtest_cryptopp
cd ..

cd mingw-O2
speedtest_custom
speedtest_cryptopp
cd ..

cd mingw-O3
speedtest_custom
speedtest_cryptopp
cd ..

cd mingw-Os
speedtest_custom
speedtest_cryptopp
cd ..

cd msvc8-Od
speedtest_custom
speedtest_cryptopp
cd ..

cd msvc8-O1
speedtest_custom
speedtest_cryptopp
cd ..

cd msvc8-O2
speedtest_custom
speedtest_cryptopp
cd ..

cd msvc8-Ox
speedtest_custom
speedtest_cryptopp
cd ..
