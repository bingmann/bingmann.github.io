// $Id: speedtest_cryptopp.cpp 222 2008-04-07 16:50:19Z tb $

#define CRYPTOPP_INCLUDE_MODES 		<CRYPTOPP_INCLUDE_PREFIX/modes.h>
#define CRYPTOPP_INCLUDE_RIJNDAEL 	<CRYPTOPP_INCLUDE_PREFIX/rijndael.h>
#define CRYPTOPP_INCLUDE_SERPENT 	<CRYPTOPP_INCLUDE_PREFIX/serpent.h>
#define CRYPTOPP_INCLUDE_TWOFISH 	<CRYPTOPP_INCLUDE_PREFIX/twofish.h>
#define CRYPTOPP_INCLUDE_CAST 		<CRYPTOPP_INCLUDE_PREFIX/cast.h>
#define CRYPTOPP_INCLUDE_GOST 		<CRYPTOPP_INCLUDE_PREFIX/gost.h>
#define CRYPTOPP_INCLUDE_TEA 		<CRYPTOPP_INCLUDE_PREFIX/tea.h>
#define CRYPTOPP_INCLUDE_BLOWFISH 	<CRYPTOPP_INCLUDE_PREFIX/blowfish.h>
#define CRYPTOPP_INCLUDE_DES	 	<CRYPTOPP_INCLUDE_PREFIX/des.h>

#include CRYPTOPP_INCLUDE_MODES
#include CRYPTOPP_INCLUDE_RIJNDAEL
#include CRYPTOPP_INCLUDE_SERPENT
#include CRYPTOPP_INCLUDE_TWOFISH
#include CRYPTOPP_INCLUDE_CAST
#include CRYPTOPP_INCLUDE_GOST
#include CRYPTOPP_INCLUDE_TEA
#include CRYPTOPP_INCLUDE_BLOWFISH
#include CRYPTOPP_INCLUDE_DES

#include "speedtest.h"

// *** Test Functions for Crypto++ ***

void test_cryptopp_rijndael_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::Rijndael>::Encryption encctx;
    encctx.SetKey(enckey, 32);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::Rijndael>::Decryption decctx;
    decctx.SetKey(enckey, 32);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_serpent_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::Serpent>::Encryption encctx;
    encctx.SetKey(enckey, 32);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::Serpent>::Decryption decctx;
    decctx.SetKey(enckey, 32);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_twofish_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::Twofish>::Encryption encctx;
    encctx.SetKey(enckey, 32);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::Twofish>::Decryption decctx;
    decctx.SetKey(enckey, 32);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_cast6_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::CAST256>::Encryption encctx;
    encctx.SetKey(enckey, 32);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::CAST256>::Decryption decctx;
    decctx.SetKey(enckey, 32);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_gost_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::GOST>::Encryption encctx;
    encctx.SetKey(enckey, 32);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::GOST>::Decryption decctx;
    decctx.SetKey(enckey, 32);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_xtea_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::XTEA>::Encryption encctx;
    encctx.SetKey(enckey, 16);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::XTEA>::Decryption decctx;
    decctx.SetKey(enckey, 16);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_blowfish_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::Blowfish>::Encryption encctx;
    encctx.SetKey(enckey, 16);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::Blowfish>::Decryption decctx;
    decctx.SetKey(enckey, 16);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_cast5_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::CAST128>::Encryption encctx;
    encctx.SetKey(enckey, 16);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::CAST128>::Decryption decctx;
    decctx.SetKey(enckey, 16);
    decctx.ProcessData(buffer, buffer, bufferlen);
}

void test_cryptopp_3des_ecb()
{
    CryptoPP::ECB_Mode<CryptoPP::DES_EDE3>::Encryption encctx;
    encctx.SetKey(enckey, 24);
    encctx.ProcessData(buffer, buffer, bufferlen);

    CryptoPP::ECB_Mode<CryptoPP::DES_EDE3>::Decryption decctx;
    decctx.SetKey(enckey, 24);
    decctx.ProcessData(buffer, buffer, bufferlen);
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

    run_test<test_cryptopp_rijndael_ecb>("cryptopp-rijndael-ecb.txt");
    run_test<test_cryptopp_serpent_ecb>("cryptopp-serpent-ecb.txt");
    run_test<test_cryptopp_twofish_ecb>("cryptopp-twofish-ecb.txt");
    run_test<test_cryptopp_cast6_ecb>("cryptopp-cast6-ecb.txt");
    run_test<test_cryptopp_gost_ecb>("cryptopp-gost-ecb.txt");
    run_test<test_cryptopp_xtea_ecb>("cryptopp-xtea-ecb.txt");
    run_test<test_cryptopp_blowfish_ecb>("cryptopp-blowfish-ecb.txt");
    run_test<test_cryptopp_cast5_ecb>("cryptopp-cast5-ecb.txt");
    run_test<test_cryptopp_3des_ecb>("cryptopp-3des-ecb.txt");
}
