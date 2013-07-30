// $Id: speedtest_custom.cpp 217 2008-04-07 14:00:30Z tb $

#include "rijndael.h"
#include "serpent-gladman.h"
#include "serpent.h"

#include "speedtest.h"

// *** Test Functions for Custom Implementation Ciphers ***

void test_my_rijndael_ecb()
{
    RijndaelEncryptECB encctx;
    encctx.set_key(enckey, 32);
    encctx.encrypt(buffer, buffer, bufferlen);

    RijndaelDecryptECB decctx;
    decctx.set_key(enckey, 32);
    decctx.decrypt(buffer, buffer, bufferlen);
}

void test_gladman_serpent_ecb()
{
    SerpentGladman::EncryptECB encctx;
    encctx.set_key(enckey, 32);
    encctx.encrypt(buffer, buffer, bufferlen);

    SerpentGladman::DecryptECB decctx;
    decctx.set_key(enckey, 32);
    decctx.decrypt(buffer, buffer, bufferlen);
}

void test_mybotan_serpent_ecb()
{
    SerpentBotan::EncryptECB encctx;
    encctx.set_key(enckey, 32);
    encctx.encrypt(buffer, buffer, bufferlen);

    SerpentBotan::DecryptECB decctx;
    decctx.set_key(enckey, 32);
    decctx.decrypt(buffer, buffer, bufferlen);
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

    run_test<test_my_rijndael_ecb>("my-rijndael-ecb.txt");
    run_test<test_gladman_serpent_ecb>("gladman-serpent-ecb.txt");
    run_test<test_mybotan_serpent_ecb>("mybotan-serpent-ecb.txt");
}
