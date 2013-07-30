// $Id: speedtest_mcrypt.cpp 216 2008-04-07 13:46:27Z tb $

#include <mcrypt.h>

#include "speedtest.h"

// *** Test Functions for libmcrypt ***

void test_libmcrypt_rijndael_ecb()
{
    // note: MCRYPT_RIJNDAEL_128 means blocksize 128 _not_ keysize 128 bits

    MCRYPT encctx = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_serpent_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_SERPENT, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_SERPENT, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_twofish_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_TWOFISH, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_TWOFISH, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_cast6_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_CAST_256, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_CAST_256, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_xtea_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_XTEA, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 16, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_XTEA, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 16, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_saferplus_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_SAFERPLUS, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_SAFERPLUS, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_loki97_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_LOKI97, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_LOKI97, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_blowfish_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_BLOWFISH, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 16, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_BLOWFISH, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 16, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_gost_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_GOST, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 32, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_GOST, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 32, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_cast5_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_CAST_128, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 16, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_CAST_128, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 16, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

void test_libmcrypt_3des_ecb()
{
    MCRYPT encctx = mcrypt_module_open(MCRYPT_3DES, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(encctx, enckey, 24, NULL);
    mcrypt_generic(encctx, buffer, bufferlen);
    mcrypt_generic_end(encctx);

    MCRYPT decctx = mcrypt_module_open(MCRYPT_3DES, NULL, MCRYPT_ECB, NULL);
    mcrypt_generic_init(decctx, enckey, 24, NULL);
    mdecrypt_generic(decctx, buffer, bufferlen);
    mcrypt_generic_end(decctx);
}

// *** main() ***

int main()
{
    // Create (somewhat) random encryption key and initialization vector

    srand(time(NULL));

    for(unsigned int i = 0; i < sizeof(enckey); ++i)
	enckey[i] = rand();

    for(unsigned int i = 0; i < sizeof(enciv); ++i)
	enciv[i] = rand();

    // Run speed tests

    run_test<test_libmcrypt_rijndael_ecb>("mcrypt-rijndael-ecb.txt");
    run_test<test_libmcrypt_serpent_ecb>("mcrypt-serpent-ecb.txt");
    run_test<test_libmcrypt_twofish_ecb>("mcrypt-twofish-ecb.txt");
    run_test<test_libmcrypt_cast6_ecb>("mcrypt-cast6-ecb.txt");
    run_test<test_libmcrypt_xtea_ecb>("mcrypt-xtea-ecb.txt");
    run_test<test_libmcrypt_saferplus_ecb>("mcrypt-saferplus-ecb.txt");
    run_test<test_libmcrypt_loki97_ecb>("mcrypt-loki97-ecb.txt");
    run_test<test_libmcrypt_blowfish_ecb>("mcrypt-blowfish-ecb.txt");
    run_test<test_libmcrypt_gost_ecb>("mcrypt-gost-ecb.txt");
    run_test<test_libmcrypt_cast5_ecb>("mcrypt-cast5-ecb.txt");
    run_test<test_libmcrypt_3des_ecb>("mcrypt-3des-ecb.txt");
}
