// $Id: speedtest_openssl.cpp 217 2008-04-07 14:00:30Z tb $

#define NCOMPAT
#include <openssl/aes.h>
#include <openssl/cast.h>
#include <openssl/blowfish.h>
#include <openssl/des.h>

#include "speedtest.h"

// *** Test Functions for OpenSSL ***

void test_openssl_rijndael_ecb()
{
    AES_KEY encctx;
    AES_set_encrypt_key(enckey, 256, &encctx);

    for(unsigned int p = 0; p < bufferlen; p += AES_BLOCK_SIZE)
	AES_encrypt(buffer + p, buffer + p, &encctx);

    AES_KEY decctx;
    AES_set_decrypt_key(enckey, 256, &decctx);

    for(unsigned int p = 0; p < bufferlen; p += AES_BLOCK_SIZE)
	AES_decrypt(buffer + p, buffer + p, &decctx);
}

void test_openssl_cast5_ecb()
{
    CAST_KEY encctx;
    CAST_set_key(&encctx, 16, enckey);

    for(unsigned int p = 0; p < bufferlen; p += CAST_BLOCK)
	CAST_encrypt((CAST_LONG*)(buffer + p), &encctx);

    CAST_KEY decctx;
    CAST_set_key(&decctx, 16, enckey);

    for(unsigned int p = 0; p < bufferlen; p += CAST_BLOCK)
	CAST_decrypt((CAST_LONG*)(buffer + p), &decctx);
}

void test_openssl_blowfish_ecb()
{
    BF_KEY encctx;
    BF_set_key(&encctx, 16, enckey);

    for(unsigned int p = 0; p < bufferlen; p += BF_BLOCK)
	BF_ecb_encrypt(buffer + p, buffer + p, &encctx, BF_ENCRYPT);

    BF_KEY decctx;
    BF_set_key(&decctx, 16, enckey);

    for(unsigned int p = 0; p < bufferlen; p += BF_BLOCK)
	BF_ecb_encrypt(buffer + p, buffer + p, &decctx, BF_DECRYPT);
}

void test_openssl_3des_ecb()
{
    DES_key_schedule eks1, eks2, eks3;

    DES_set_key((DES_cblock*)(enckey +  0), &eks1);
    DES_set_key((DES_cblock*)(enckey +  8), &eks2);
    DES_set_key((DES_cblock*)(enckey + 16), &eks3);

    for(unsigned int p = 0; p < bufferlen; p += 8)
	DES_encrypt3((DES_LONG*)(buffer + p), &eks1, &eks2, &eks3);

    DES_key_schedule dks1, dks2, dks3;

    DES_set_key((DES_cblock*)(enckey +  0), &dks1);
    DES_set_key((DES_cblock*)(enckey +  8), &dks2);
    DES_set_key((DES_cblock*)(enckey + 16), &dks3);

    for(unsigned int p = 0; p < bufferlen; p += 8)
	DES_decrypt3((DES_LONG*)(buffer + p), &dks1, &dks2, &dks3);
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

    run_test<test_openssl_rijndael_ecb>("openssl-rijndael-ecb.txt");
    run_test<test_openssl_cast5_ecb>("openssl-cast5-ecb.txt");
    run_test<test_openssl_blowfish_ecb>("openssl-blowfish-ecb.txt");
    run_test<test_openssl_3des_ecb>("openssl-3des-ecb.txt");
}
