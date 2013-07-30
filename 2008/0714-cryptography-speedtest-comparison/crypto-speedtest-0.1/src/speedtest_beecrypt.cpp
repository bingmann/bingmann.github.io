// $Id: speedtest_beecrypt.cpp 219 2008-04-07 14:22:31Z tb $

#include <beecrypt/aes.h>
#include <beecrypt/blowfish.h>

#include "speedtest.h"

// *** Test Functions for Beecrypt ***

void test_beecrypt_rijndael_ecb()
{
    aesParam encctx;
    aesSetup(&encctx, (byte*)enckey, 256, ENCRYPT);

    for(unsigned int p = 0; p < bufferlen; p += 16)
	aesEncrypt(&encctx, (uint32_t*)(buffer + p), (uint32_t*)(buffer + p));

    aesParam decctx;
    aesSetup(&decctx, (byte*)enckey, 256, DECRYPT);

    for(unsigned int p = 0; p < bufferlen; p += 16)
	aesDecrypt(&decctx, (uint32_t*)(buffer + p), (uint32_t*)(buffer + p));
}

void test_beecrypt_blowfish_ecb()
{
    blowfishParam encctx;
    blowfishSetup(&encctx, (byte*)enckey, 128, ENCRYPT);

    for(unsigned int p = 0; p < bufferlen; p += 8)
	blowfishEncrypt(&encctx, (uint32_t*)(buffer + p), (uint32_t*)(buffer + p));

    blowfishParam decctx;
    blowfishSetup(&decctx, (byte*)enckey, 128, DECRYPT);

    for(unsigned int p = 0; p < bufferlen; p += 8)
	blowfishDecrypt(&decctx, (uint32_t*)(buffer + p), (uint32_t*)(buffer + p));
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

    run_test<test_beecrypt_rijndael_ecb>("beecrypt-rijndael-ecb.txt");
    run_test<test_beecrypt_blowfish_ecb>("beecrypt-blowfish-ecb.txt");
}
