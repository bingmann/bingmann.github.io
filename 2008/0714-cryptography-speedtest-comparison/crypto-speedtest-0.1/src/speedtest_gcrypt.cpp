// $Id: speedtest_gcrypt.cpp 217 2008-04-07 14:00:30Z tb $

#include <gcrypt.h>

#include "speedtest.h"

// *** Test Functions for libgcrypt ***

void test_libgcrypt_rijndael_ecb()
{
    gcry_cipher_hd_t encctx;
    gcry_cipher_open(&encctx, GCRY_CIPHER_RIJNDAEL256, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(encctx, enckey, 32);
    gcry_cipher_encrypt(encctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(encctx);

    gcry_cipher_hd_t decctx;
    gcry_cipher_open(&decctx, GCRY_CIPHER_RIJNDAEL256, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(decctx, enckey, 32);
    gcry_cipher_decrypt(decctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(decctx);
}

void test_libgcrypt_serpent_ecb()
{
    gcry_cipher_hd_t encctx;
    gcry_cipher_open(&encctx, GCRY_CIPHER_SERPENT256, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(encctx, enckey, 32);
    gcry_cipher_encrypt(encctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(encctx);

    gcry_cipher_hd_t decctx;
    gcry_cipher_open(&decctx, GCRY_CIPHER_SERPENT256, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(decctx, enckey, 32);
    gcry_cipher_decrypt(decctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(decctx);
}

void test_libgcrypt_twofish_ecb()
{
    gcry_cipher_hd_t encctx;
    gcry_cipher_open(&encctx, GCRY_CIPHER_TWOFISH, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(encctx, enckey, 32);
    gcry_cipher_encrypt(encctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(encctx);

    gcry_cipher_hd_t decctx;
    gcry_cipher_open(&decctx, GCRY_CIPHER_TWOFISH, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(decctx, enckey, 32);
    gcry_cipher_decrypt(decctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(decctx);
}

void test_libgcrypt_blowfish_ecb()
{
    gcry_cipher_hd_t encctx;
    gcry_cipher_open(&encctx, GCRY_CIPHER_BLOWFISH, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(encctx, enckey, 16);
    gcry_cipher_encrypt(encctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(encctx);

    gcry_cipher_hd_t decctx;
    gcry_cipher_open(&decctx, GCRY_CIPHER_BLOWFISH, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(decctx, enckey, 16);
    gcry_cipher_decrypt(decctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(decctx);
}

void test_libgcrypt_cast5_ecb()
{
    gcry_cipher_hd_t encctx;
    gcry_cipher_open(&encctx, GCRY_CIPHER_CAST5, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(encctx, enckey, 16);
    gcry_cipher_encrypt(encctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(encctx);

    gcry_cipher_hd_t decctx;
    gcry_cipher_open(&decctx, GCRY_CIPHER_CAST5, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(decctx, enckey, 16);
    gcry_cipher_decrypt(decctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(decctx);
}

void test_libgcrypt_3des_ecb()
{
    gcry_cipher_hd_t encctx;
    gcry_cipher_open(&encctx, GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(encctx, enckey, 24);
    gcry_cipher_encrypt(encctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(encctx);

    gcry_cipher_hd_t decctx;
    gcry_cipher_open(&decctx, GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(decctx, enckey, 24);
    gcry_cipher_decrypt(decctx, buffer, bufferlen, buffer, bufferlen);
    gcry_cipher_close(decctx);
}

// *** main() ***

int main()
{
    // Initialize cryptographic library

    gcry_check_version(GCRYPT_VERSION);

    // Create (somewhat) random encryption key and initialization vector

    srand(time(NULL));

    for(unsigned int i = 0; i < sizeof(enckey); ++i)
	enckey[i] = rand();

    for(unsigned int i = 0; i < sizeof(enciv); ++i)
	enciv[i] = rand();

    // Run speed tests

    run_test<test_libgcrypt_rijndael_ecb>("gcrypt-rijndael-ecb.txt");
    run_test<test_libgcrypt_serpent_ecb>("gcrypt-serpent-ecb.txt");
    run_test<test_libgcrypt_twofish_ecb>("gcrypt-twofish-ecb.txt");
    run_test<test_libgcrypt_blowfish_ecb>("gcrypt-blowfish-ecb.txt");
    run_test<test_libgcrypt_cast5_ecb>("gcrypt-cast5-ecb.txt");
    run_test<test_libgcrypt_3des_ecb>("gcrypt-3des-ecb.txt");
}
