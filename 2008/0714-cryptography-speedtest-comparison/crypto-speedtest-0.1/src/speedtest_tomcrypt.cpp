// $Id: speedtest_tomcrypt.cpp 225 2008-04-09 17:08:35Z tb $

#include <tomcrypt.h>

#include "speedtest.h"

// *** Test Functions for libtomcrypt ***

void test_tomcrypt_rijndael_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("rijndael"), enckey, 32, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("rijndael"), enckey, 32, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_twofish_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("twofish"), enckey, 32, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("twofish"), enckey, 32, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_saferplus_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("safer+"), enckey, 32, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("safer+"), enckey, 32, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_noekeon_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("noekeon"), enckey, 16, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("noekeon"), enckey, 16, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_skipjack_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("skipjack"), enckey, 10, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("skipjack"), enckey, 10, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_anubis_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("anubis"), enckey, 32, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("anubis"), enckey, 32, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_khazad_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("khazad"), enckey, 16, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("khazad"), enckey, 16, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_xtea_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("xtea"), enckey, 16, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("xtea"), enckey, 16, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_blowfish_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("blowfish"), enckey, 16, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("blowfish"), enckey, 16, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_cast5_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("cast5"), enckey, 16, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("cast5"), enckey, 16, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

void test_tomcrypt_3des_ecb()
{
    symmetric_ECB encctx;
    ecb_start(find_cipher("3des"), enckey, 24, 0, &encctx);
    ecb_encrypt(buffer, buffer, bufferlen, &encctx);
    ecb_done(&encctx);

    symmetric_ECB decctx;
    ecb_start(find_cipher("3des"), enckey, 24, 0, &decctx);
    ecb_decrypt(buffer, buffer, bufferlen, &decctx);
    ecb_done(&decctx);
}

// *** main() ***

int main()
{
    // Initialize cryptographic library

    register_cipher(&rijndael_desc);
    register_cipher(&twofish_desc);
    register_cipher(&saferp_desc);
    register_cipher(&noekeon_desc);
    register_cipher(&skipjack_desc);
    register_cipher(&anubis_desc);
    register_cipher(&khazad_desc);
    register_cipher(&xtea_desc);
    register_cipher(&blowfish_desc);
    register_cipher(&cast5_desc);
    register_cipher(&des3_desc);

    // Create (somewhat) random encryption key and initialization vector

    srand(time(NULL));

    for(unsigned int i = 0; i < sizeof(enckey); ++i)
	enckey[i] = rand();

    for(unsigned int i = 0; i < sizeof(enciv); ++i)
	enciv[i] = rand();

    // Run speed tests

    run_test<test_tomcrypt_rijndael_ecb>("tomcrypt-rijndael-ecb.txt");
    run_test<test_tomcrypt_twofish_ecb>("tomcrypt-twofish-ecb.txt");
    run_test<test_tomcrypt_saferplus_ecb>("tomcrypt-saferplus-ecb.txt");
    run_test<test_tomcrypt_noekeon_ecb>("tomcrypt-noekeon-ecb.txt");
    run_test<test_tomcrypt_skipjack_ecb>("tomcrypt-skipjack-ecb.txt");
    run_test<test_tomcrypt_anubis_ecb>("tomcrypt-anubis-ecb.txt");
    run_test<test_tomcrypt_khazad_ecb>("tomcrypt-khazad-ecb.txt");
    run_test<test_tomcrypt_xtea_ecb>("tomcrypt-xtea-ecb.txt");
    run_test<test_tomcrypt_blowfish_ecb>("tomcrypt-blowfish-ecb.txt");
    run_test<test_tomcrypt_cast5_ecb>("tomcrypt-cast5-ecb.txt");
    run_test<test_tomcrypt_3des_ecb>("tomcrypt-3des-ecb.txt");
}
