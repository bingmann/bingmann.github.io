// $Id: speedtest_botan.cpp 217 2008-04-07 14:00:30Z tb $

#include <botan/botan.h>
#include <botan/aes.h>
#include <botan/serpent.h>
#include <botan/twofish.h>
#include <botan/cast256.h>
#include <botan/gost.h>
#include <botan/xtea.h>
#include <botan/blowfish.h>
#include <botan/cast128.h>
#include <botan/des.h>

#include "speedtest.h"

// *** Test Functions for Botan ***

void test_botan_rijndael_ecb()
{
    Botan::AES_256 encctx;
    encctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::AES_256 decctx;
    decctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_serpent_ecb()
{
    Botan::Serpent encctx;
    encctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::Serpent decctx;
    decctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_twofish_ecb()
{
    Botan::Twofish encctx;
    encctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::Twofish decctx;
    decctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_cast6_ecb()
{
    Botan::CAST_256 encctx;
    encctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::CAST_256 decctx;
    decctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_gost_ecb()
{
    Botan::GOST encctx;
    encctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::GOST decctx;
    decctx.set_key(enckey, 32);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_xtea_ecb()
{
    Botan::XTEA encctx;
    encctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::XTEA decctx;
    decctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_blowfish_ecb()
{
    Botan::Blowfish encctx;
    encctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::Blowfish decctx;
    decctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_cast5_ecb()
{
    Botan::CAST_128 encctx;
    encctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::CAST_128 decctx;
    decctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

void test_botan_3des_ecb()
{
    Botan::TripleDES encctx;
    encctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
	encctx.encrypt(buffer + p);

    Botan::TripleDES decctx;
    decctx.set_key(enckey, 16);

    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
	decctx.decrypt(buffer + p);
}

// *** main() ***

int main()
{
    // Initialize cryptographic library

    Botan::LibraryInitializer init;

    // Create (somewhat) random encryption key and initialization vector

    srand(time(NULL));

    for(unsigned int i = 0; i < sizeof(enckey); ++i)
	enckey[i] = rand();

    for(unsigned int i = 0; i < sizeof(enciv); ++i)
	enciv[i] = rand();

    // Run speed tests

    run_test<test_botan_rijndael_ecb>("botan-rijndael-ecb.txt");
    run_test<test_botan_serpent_ecb>("botan-serpent-ecb.txt");
    run_test<test_botan_twofish_ecb>("botan-twofish-ecb.txt");
    run_test<test_botan_cast6_ecb>("botan-cast6-ecb.txt");
    run_test<test_botan_gost_ecb>("botan-gost-ecb.txt");
    run_test<test_botan_xtea_ecb>("botan-xtea-ecb.txt");
    run_test<test_botan_blowfish_ecb>("botan-blowfish-ecb.txt");
    run_test<test_botan_cast5_ecb>("botan-cast5-ecb.txt");
    run_test<test_botan_3des_ecb>("botan-3des-ecb.txt");
}
