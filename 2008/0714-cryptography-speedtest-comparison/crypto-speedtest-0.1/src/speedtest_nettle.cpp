// $Id: speedtest_nettle.cpp 219 2008-04-07 14:22:31Z tb $

extern "C" {
#include <nettle/aes.h>
#include <nettle/serpent.h>
#include <nettle/twofish.h>
#include <nettle/cast128.h>
#include <nettle/blowfish.h>
#include <nettle/des.h>
}

#include "speedtest.h"

// *** Test Functions for Nettle ***

void test_nettle_rijndael_ecb()
{
    aes_ctx encctx;
    aes_set_encrypt_key(&encctx, 32, enckey);
    aes_encrypt(&encctx, bufferlen, buffer, buffer);

    aes_ctx decctx;
    aes_set_decrypt_key(&decctx, 32, enckey);
    aes_decrypt(&decctx, bufferlen, buffer, buffer);
}

void test_nettle_serpent_ecb()
{
    serpent_ctx encctx;
    serpent_set_key(&encctx, 32, enckey);
    serpent_encrypt(&encctx, bufferlen, buffer, buffer);

    serpent_ctx decctx;
    serpent_set_key(&decctx, 32, enckey);
    serpent_decrypt(&decctx, bufferlen, buffer, buffer);
}

void test_nettle_twofish_ecb()
{
    twofish_ctx encctx;
    twofish_set_key(&encctx, 32, enckey);
    twofish_encrypt(&encctx, bufferlen, buffer, buffer);

    twofish_ctx decctx;
    twofish_set_key(&decctx, 32, enckey);
    twofish_decrypt(&decctx, bufferlen, buffer, buffer);
}

void test_nettle_cast5_ecb()
{
    cast128_ctx encctx;
    cast128_set_key(&encctx, 16, enckey);
    cast128_encrypt(&encctx, bufferlen, buffer, buffer);

    cast128_ctx decctx;
    cast128_set_key(&decctx, 16, enckey);
    cast128_decrypt(&decctx, bufferlen, buffer, buffer);
}

void test_nettle_blowfish_ecb()
{
    blowfish_ctx encctx;
    blowfish_set_key(&encctx, 16, enckey);
    blowfish_encrypt(&encctx, bufferlen, buffer, buffer);

    blowfish_ctx decctx;
    blowfish_set_key(&decctx, 16, enckey);
    blowfish_decrypt(&decctx, bufferlen, buffer, buffer);
}

void test_nettle_3des_ecb()
{
    des3_ctx encctx;
    des3_set_key(&encctx, enckey);
    des3_encrypt(&encctx, bufferlen, buffer, buffer);

    des3_ctx decctx;
    des3_set_key(&decctx, enckey);
    des3_decrypt(&decctx, bufferlen, buffer, buffer);
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

    // Nettle requires some parity fix of the key
    des_fix_parity(24, enckey, enckey);

    // Run speed tests

    run_test<test_nettle_rijndael_ecb>("nettle-rijndael-ecb.txt");
    run_test<test_nettle_serpent_ecb>("nettle-serpent-ecb.txt");
    run_test<test_nettle_twofish_ecb>("nettle-twofish-ecb.txt");
    run_test<test_nettle_blowfish_ecb>("nettle-blowfish-ecb.txt");
    run_test<test_nettle_cast5_ecb>("nettle-cast5-ecb.txt");
    run_test<test_nettle_3des_ecb>("nettle-3des-ecb.txt");
}
