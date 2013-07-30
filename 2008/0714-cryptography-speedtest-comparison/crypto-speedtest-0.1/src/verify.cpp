// $Id: verify.cpp 222 2008-04-07 16:50:19Z tb $

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <assert.h>
#include <map>
#include <vector>
#include <numeric>
#include <math.h>

#if HAVE_LIBTOMCRYPT
#include <tomcrypt.h>
// remove some macros slashing with other libraries
#undef byte
#undef XTEA
#undef DES
#endif

#if HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

#if HAVE_LIBMCRYPT
#include <mcrypt.h>
#endif

#if HAVE_BOTAN
#include <botan/botan.h>
#include <botan/aes.h>
#include <botan/serpent.h>
#include <botan/twofish.h>
#include <botan/blowfish.h>
#include <botan/des.h>
#endif

#if HAVE_CRYPTOPP

#define CRYPTOPP_INCLUDE_MODES 		<CRYPTOPP_INCLUDE_PREFIX/modes.h>
#define CRYPTOPP_INCLUDE_RIJNDAEL 	<CRYPTOPP_INCLUDE_PREFIX/rijndael.h>
#define CRYPTOPP_INCLUDE_SERPENT 	<CRYPTOPP_INCLUDE_PREFIX/serpent.h>
#define CRYPTOPP_INCLUDE_TWOFISH 	<CRYPTOPP_INCLUDE_PREFIX/twofish.h>
#define CRYPTOPP_INCLUDE_BLOWFISH 	<CRYPTOPP_INCLUDE_PREFIX/blowfish.h>
#define CRYPTOPP_INCLUDE_DES	 	<CRYPTOPP_INCLUDE_PREFIX/des.h>

#include CRYPTOPP_INCLUDE_MODES
#include CRYPTOPP_INCLUDE_RIJNDAEL
#include CRYPTOPP_INCLUDE_SERPENT
#include CRYPTOPP_INCLUDE_TWOFISH
#include CRYPTOPP_INCLUDE_BLOWFISH
#include CRYPTOPP_INCLUDE_DES

#endif

#if HAVE_OPENSSL
#define NCOMPAT
#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/blowfish.h>
#endif

#if HAVE_LIBNETTLE
extern "C" {
#include <nettle/aes.h>
#include <nettle/serpent.h>
#include <nettle/blowfish.h>
#include <nettle/des.h>
}
#endif

#if HAVE_LIBBEECRYPT
#include <beecrypt/aes.h>
#endif

#include "rijndael.h"
#include "serpent-gladman.h"
#include "serpent.h"

// *** Verfication Parameters ***

const unsigned int bufferlen = 8192*16;

// *** Global Buffers and Settings for the Verify Functions ***

unsigned char enckey[32];

// *** Tool Functions to Fill, Check and Compare Buffers ***

void fill_buffer(uint8_t *buffer, unsigned int bufferlen)
{
    for(unsigned int i = 0; i < bufferlen; ++i)
	buffer[i] = (uint8_t)i;
}

void compare_buffers(uint8_t *buffer1, uint8_t* buffer2, unsigned int bufferlen)
{
    for(unsigned int i = 0; i < bufferlen; ++i)
	assert(buffer1[i] == buffer2[i]);
}

void check_buffer(uint8_t *buffer, unsigned int bufferlen)
{
    for(unsigned int i = 0; i < bufferlen; ++i)
	assert(buffer[i] == (uint8_t)i);
}

// *** Verify Rijndael Implementations

void verify_rijndael_ecb()
{
    std::cout << "Testing Rijndael AES implementations\n";

    // Custom Implementation

    uint8_t buffer_custom[bufferlen];
    fill_buffer(buffer_custom, bufferlen);

    {
	RijndaelEncryptECB encctx;
	encctx.set_key(enckey, 32);
	encctx.encrypt(buffer_custom, buffer_custom, bufferlen);
    }

#if HAVE_LIBGCRYPT
    {
	std::cout << "libgcrypt\n";

	uint8_t buffer_gcrypt[bufferlen];
	fill_buffer(buffer_gcrypt, bufferlen);

	{
	    gcry_cipher_hd_t encctx;
	    gcry_cipher_open(&encctx, GCRY_CIPHER_RIJNDAEL256, GCRY_CIPHER_MODE_ECB, 0);
	    gcry_cipher_setkey(encctx, enckey, 32);
	    gcry_cipher_encrypt(encctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	    gcry_cipher_close(encctx);
	}

	compare_buffers(buffer_custom, buffer_gcrypt, bufferlen);

	{
	    gcry_cipher_hd_t decctx;
	    gcry_cipher_open(&decctx, GCRY_CIPHER_RIJNDAEL256, GCRY_CIPHER_MODE_ECB, 0);
	    gcry_cipher_setkey(decctx, enckey, 32);
	    gcry_cipher_decrypt(decctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	    gcry_cipher_close(decctx);
	}

	check_buffer(buffer_gcrypt, bufferlen);
    }
#endif

#if HAVE_LIBMCRYPT
    {
	std::cout << "libmcrypt\n";

	uint8_t buffer_mcrypt[bufferlen];
	fill_buffer(buffer_mcrypt, bufferlen);

	{
	    MCRYPT encctx = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(encctx, enckey, 32, NULL);
	    mcrypt_generic(encctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(encctx);
	}

	compare_buffers(buffer_custom, buffer_mcrypt, bufferlen);

	{
	    MCRYPT decctx = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(decctx, enckey, 32, NULL);
	    mdecrypt_generic(decctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(decctx);
	}

	check_buffer(buffer_mcrypt, bufferlen);
    }
#endif

#if HAVE_BOTAN
    {
	std::cout << "botan\n";

	uint8_t buffer_botan[bufferlen];
	fill_buffer(buffer_botan, bufferlen);

	{
	    Botan::AES_256 encctx;
	    encctx.set_key((Botan::byte*)enckey, 32);

	    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
		encctx.encrypt((Botan::byte*)buffer_botan + p);
	}

	compare_buffers(buffer_custom, buffer_botan, bufferlen);

	{
	    Botan::AES_256 decctx;
	    decctx.set_key((Botan::byte*)enckey, 32);

	    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
		decctx.decrypt((Botan::byte*)buffer_botan + p);
	}

	check_buffer(buffer_botan, bufferlen);
    }
#endif

#if HAVE_CRYPTOPP
    {
	std::cout << "crypto++\n";

	uint8_t buffer_cryptopp[bufferlen];
	fill_buffer(buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Rijndael>::Encryption encctx;
	    encctx.SetKey(enckey, 32);

	    encctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	compare_buffers(buffer_custom, buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Rijndael>::Decryption decctx;
	    decctx.SetKey(enckey, 32);

	    decctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	check_buffer(buffer_cryptopp, bufferlen);
    }
#endif

#if HAVE_OPENSSL
    {
	std::cout << "openssl\n";

	uint8_t buffer_openssl[bufferlen];
	fill_buffer(buffer_openssl, bufferlen);

	{
	    AES_KEY aeskey;
	    AES_set_encrypt_key(enckey, 256, &aeskey);

	    for(unsigned int p = 0; p < bufferlen; p += AES_BLOCK_SIZE)
		AES_encrypt(buffer_openssl + p, buffer_openssl + p, &aeskey);
	}

	compare_buffers(buffer_custom, buffer_openssl, bufferlen);

	{
	    AES_KEY aeskey;
	    AES_set_decrypt_key(enckey, 256, &aeskey);

	    for(unsigned int p = 0; p < bufferlen; p += AES_BLOCK_SIZE)
		AES_decrypt(buffer_openssl + p, buffer_openssl + p, &aeskey);
	}

	check_buffer(buffer_openssl, bufferlen);
    }
#endif

#if HAVE_LIBNETTLE
    {
	std::cout << "nettle\n";

	uint8_t buffer_nettle[bufferlen];
	fill_buffer(buffer_nettle, bufferlen);

	{
	    aes_ctx encctx;
	    aes_set_encrypt_key(&encctx, 32, enckey);
	    aes_encrypt(&encctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	compare_buffers(buffer_custom, buffer_nettle, bufferlen);

	{
	    aes_ctx decctx;
	    aes_set_decrypt_key(&decctx, 32, enckey);
	    aes_decrypt(&decctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	check_buffer(buffer_nettle, bufferlen);
    }
#endif

#if HAVE_LIBBEECRYPT
    {
	std::cout << "beecrypt\n";

	uint8_t buffer_beecrypt[bufferlen];
	fill_buffer(buffer_beecrypt, bufferlen);

	{
	    aesParam encctx;
	    aesSetup(&encctx, enckey, 256, ENCRYPT);

	    for(unsigned int p = 0; p < bufferlen; p += 16)
		aesEncrypt(&encctx, (uint32_t*)(buffer_beecrypt + p), (uint32_t*)(buffer_beecrypt + p));
	}

	compare_buffers(buffer_custom, buffer_beecrypt, bufferlen);

	{
	    aesParam decctx;
	    aesSetup(&decctx, enckey, 256, DECRYPT);

	    for(unsigned int p = 0; p < bufferlen; p += 16)
		aesDecrypt(&decctx, (uint32_t*)(buffer_beecrypt + p), (uint32_t*)(buffer_beecrypt + p));
	}

	check_buffer(buffer_beecrypt, bufferlen);
    }
#endif

#if HAVE_LIBTOMCRYPT
    {
	std::cout << "tomcrypt\n";

	uint8_t buffer_tomcrypt[bufferlen];
	fill_buffer(buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB encctx;
	    ecb_start(find_cipher("rijndael"), enckey, 32, 0, &encctx);
	    ecb_encrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &encctx);
	    ecb_done(&encctx);
	}

	compare_buffers(buffer_custom, buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB decctx;
	    ecb_start(find_cipher("rijndael"), enckey, 32, 0, &decctx);
	    ecb_decrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &decctx);
	    ecb_done(&decctx);
	}

	check_buffer(buffer_tomcrypt, bufferlen);
    }
#endif

    // Custom Implementation

    {
	std::cout << "custom\n";

	RijndaelDecryptECB decctx;
	decctx.set_key(enckey, 32);
	decctx.decrypt(buffer_custom, buffer_custom, bufferlen);
    }

    check_buffer(buffer_custom, bufferlen);
}

// *** Verify Serpent Implementations

void verify_serpent_ecb()
{
    std::cout << "Testing Serpent implementations\n";

    // gladman implementation

    uint8_t buffer_gladman[bufferlen];
    fill_buffer(buffer_gladman, bufferlen);

    {
	SerpentGladman::EncryptECB encctx;

	encctx.set_key(enckey, 256);
	encctx.encrypt(buffer_gladman, buffer_gladman, bufferlen);
    }

    { // botan-extracted implementation
	std::cout << "mybotan\n";

	uint8_t buffer_mybotan[bufferlen];
	fill_buffer(buffer_mybotan, bufferlen);

	{
	    SerpentBotan::EncryptECB encctx;

	    encctx.set_key(enckey, 32);
	    encctx.encrypt(buffer_mybotan, buffer_mybotan, bufferlen);
	}

	compare_buffers(buffer_gladman, buffer_mybotan, bufferlen);

	{
	    SerpentBotan::DecryptECB decctx;

	    decctx.set_key(enckey, 32);
	    decctx.decrypt(buffer_mybotan, buffer_mybotan, bufferlen);
	}

	check_buffer(buffer_mybotan, bufferlen);
    }

#if HAVE_LIBGCRYPT
    {
	std::cout << "libgcrypt\n";
	
	uint8_t buffer_gcrypt[bufferlen];
	fill_buffer(buffer_gcrypt, bufferlen);

	{
	    gcry_cipher_hd_t encctx;
	    gcry_cipher_open(&encctx, GCRY_CIPHER_SERPENT256, GCRY_CIPHER_MODE_ECB, 0);
	    gcry_cipher_setkey(encctx, enckey, 32);
	    gcry_cipher_encrypt(encctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	    gcry_cipher_close(encctx);
	}

	compare_buffers(buffer_gladman, buffer_gcrypt, bufferlen);

	{
	    gcry_cipher_hd_t decctx;
	    gcry_cipher_open(&decctx, GCRY_CIPHER_SERPENT256, GCRY_CIPHER_MODE_ECB, 0);
	    gcry_cipher_setkey(decctx, enckey, 32);
	    gcry_cipher_decrypt(decctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	    gcry_cipher_close(decctx);
	}

	check_buffer(buffer_gcrypt, bufferlen);
    }
#endif

#if HAVE_LIBMCRYPT
    {
	std::cout << "libmcrypt\n";

	uint8_t buffer_mcrypt[bufferlen];
	fill_buffer(buffer_mcrypt, bufferlen);

	{
	    MCRYPT encctx = mcrypt_module_open(MCRYPT_SERPENT, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(encctx, enckey, 32, NULL);
	    mcrypt_generic(encctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(encctx);
	}

	compare_buffers(buffer_gladman, buffer_mcrypt, bufferlen);

	{
	    MCRYPT decctx = mcrypt_module_open(MCRYPT_SERPENT, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(decctx, enckey, 32, NULL);
	    mdecrypt_generic(decctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(decctx);
	}

	check_buffer(buffer_mcrypt, bufferlen);
    }
#endif

#if HAVE_BOTAN
    {
	std::cout << "botan\n";

	uint8_t buffer_botan[bufferlen];
	fill_buffer(buffer_botan, bufferlen);

	{
	    Botan::Serpent encctx;
	    encctx.set_key((Botan::byte*)enckey, 32);

	    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
		encctx.encrypt((Botan::byte*)buffer_botan + p);
	}

	compare_buffers(buffer_gladman, buffer_botan, bufferlen);

	{
	    Botan::Serpent decctx;
	    decctx.set_key((Botan::byte*)enckey, 32);

	    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
		decctx.decrypt((Botan::byte*)buffer_botan + p);
	}

	check_buffer(buffer_botan, bufferlen);
    }
#endif

#if HAVE_CRYPTOPP
    {
	std::cout << "crypto++\n";

	uint8_t buffer_cryptopp[bufferlen];
	fill_buffer(buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Serpent>::Encryption encctx;
	    encctx.SetKey(enckey, 32);

	    encctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	compare_buffers(buffer_gladman, buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Serpent>::Decryption decctx;
	    decctx.SetKey(enckey, 32);

	    decctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	check_buffer(buffer_cryptopp, bufferlen);
    }
#endif

#if HAVE_LIBNETTLE
    {
	std::cout << "nettle\n";

	uint8_t buffer_nettle[bufferlen];
	fill_buffer(buffer_nettle, bufferlen);

	{
	    serpent_ctx encctx;
	    serpent_set_key(&encctx, 32, enckey);
	    serpent_encrypt(&encctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	// does not match!
	// compare_buffers(buffer_gladman, buffer_nettle, bufferlen);

	{
	    serpent_ctx decctx;
	    serpent_set_key(&decctx, 32, enckey);
	    serpent_decrypt(&decctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	check_buffer(buffer_nettle, bufferlen);
    }
#endif

    // gladman implementation

    {
	std::cout << "gladman\n";

	SerpentGladman::DecryptECB decctx;

	decctx.set_key(enckey, 256);
	decctx.decrypt(buffer_gladman, buffer_gladman, bufferlen);
    }

    check_buffer(buffer_gladman, bufferlen);
}

// *** Verify Twofish Implementations

void verify_twofish_ecb()
{
    std::cout << "Testing Twofish implementations\n";

#if HAVE_LIBGCRYPT
    // libgcrypt

    uint8_t buffer_gcrypt[bufferlen];
    fill_buffer(buffer_gcrypt, bufferlen);

    {
	gcry_cipher_hd_t encctx;
	gcry_cipher_open(&encctx, GCRY_CIPHER_TWOFISH, GCRY_CIPHER_MODE_ECB, 0);
	gcry_cipher_setkey(encctx, enckey, 32);
	gcry_cipher_encrypt(encctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	gcry_cipher_close(encctx);
    }

#if HAVE_LIBMCRYPT
    {
	std::cout << "libmcrypt\n";

	uint8_t buffer_mcrypt[bufferlen];
	fill_buffer(buffer_mcrypt, bufferlen);

	{
	    MCRYPT encctx = mcrypt_module_open(MCRYPT_TWOFISH, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(encctx, enckey, 32, NULL);
	    mcrypt_generic(encctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(encctx);
	}

	compare_buffers(buffer_gcrypt, buffer_mcrypt, bufferlen);

	{
	    MCRYPT decctx = mcrypt_module_open(MCRYPT_TWOFISH, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(decctx, enckey, 32, NULL);
	    mdecrypt_generic(decctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(decctx);
	}

	check_buffer(buffer_mcrypt, bufferlen);
    }
#endif

#if HAVE_BOTAN
    {
	std::cout << "botan\n";

	uint8_t buffer_botan[bufferlen];
	fill_buffer(buffer_botan, bufferlen);

	{
	    Botan::Twofish encctx;
	    encctx.set_key((Botan::byte*)enckey, 32);

	    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
		encctx.encrypt((Botan::byte*)buffer_botan + p);
	}

	compare_buffers(buffer_gcrypt, buffer_botan, bufferlen);

	{
	    Botan::Twofish decctx;
	    decctx.set_key((Botan::byte*)enckey, 32);

	    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
		decctx.decrypt((Botan::byte*)buffer_botan + p);
	}

	check_buffer(buffer_botan, bufferlen);
    }
#endif

#if HAVE_CRYPTOPP
    {
	std::cout << "crypto++\n";

	uint8_t buffer_cryptopp[bufferlen];
	fill_buffer(buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Twofish>::Encryption encctx;
	    encctx.SetKey(enckey, 32);

	    encctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	compare_buffers(buffer_gcrypt, buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Twofish>::Decryption decctx;
	    decctx.SetKey(enckey, 32);

	    decctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	check_buffer(buffer_cryptopp, bufferlen);
    }
#endif

#if HAVE_LIBTOMCRYPT
    {
	std::cout << "tomcrypt\n";

	uint8_t buffer_tomcrypt[bufferlen];
	fill_buffer(buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB encctx;
	    ecb_start(find_cipher("twofish"), enckey, 32, 0, &encctx);
	    ecb_encrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &encctx);
	    ecb_done(&encctx);
	}

	compare_buffers(buffer_gcrypt, buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB decctx;
	    ecb_start(find_cipher("twofish"), enckey, 32, 0, &decctx);
	    ecb_decrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &decctx);
	    ecb_done(&decctx);
	}

	check_buffer(buffer_tomcrypt, bufferlen);
    }
#endif

    {
	std::cout << "libgcrypt\n";

	gcry_cipher_hd_t decctx;
	gcry_cipher_open(&decctx, GCRY_CIPHER_TWOFISH, GCRY_CIPHER_MODE_ECB, 0);
	gcry_cipher_setkey(decctx, enckey, 32);
	gcry_cipher_decrypt(decctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	gcry_cipher_close(decctx);
    }

    check_buffer(buffer_gcrypt, bufferlen);

#endif // HAVE_LIBGCRYPT
}

// *** Verify Blowfish Implementations

void verify_blowfish_ecb()
{
    std::cout << "Testing Blowfish implementations\n";

#if HAVE_LIBGCRYPT
    // libgcrypt

    uint8_t buffer_gcrypt[bufferlen];
    fill_buffer(buffer_gcrypt, bufferlen);

    {
	gcry_cipher_hd_t encctx;
	gcry_cipher_open(&encctx, GCRY_CIPHER_BLOWFISH, GCRY_CIPHER_MODE_ECB, 0);
	gcry_cipher_setkey(encctx, enckey, 16);
	gcry_cipher_encrypt(encctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	gcry_cipher_close(encctx);
    }

#if HAVE_LIBMCRYPT
    {
	std::cout << "libmcrypt\n";

	uint8_t buffer_mcrypt[bufferlen];
	fill_buffer(buffer_mcrypt, bufferlen);

	{
	    MCRYPT encctx = mcrypt_module_open(MCRYPT_BLOWFISH, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(encctx, enckey, 16, NULL);
	    mcrypt_generic(encctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(encctx);
	}

	compare_buffers(buffer_gcrypt, buffer_mcrypt, bufferlen);

	{
	    MCRYPT decctx = mcrypt_module_open(MCRYPT_BLOWFISH, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(decctx, enckey, 16, NULL);
	    mdecrypt_generic(decctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(decctx);
	}

	check_buffer(buffer_mcrypt, bufferlen);
    }
#endif

#if HAVE_BOTAN
    {
	std::cout << "botan\n";

	uint8_t buffer_botan[bufferlen];
	fill_buffer(buffer_botan, bufferlen);

	{
	    Botan::Blowfish encctx;
	    encctx.set_key((Botan::byte*)enckey, 16);

	    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
		encctx.encrypt((Botan::byte*)buffer_botan + p);
	}

	compare_buffers(buffer_gcrypt, buffer_botan, bufferlen);

	{
	    Botan::Blowfish decctx;
	    decctx.set_key((Botan::byte*)enckey, 16);

	    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
		decctx.decrypt((Botan::byte*)buffer_botan + p);
	}

	check_buffer(buffer_botan, bufferlen);
    }
#endif

#if HAVE_CRYPTOPP
    {
	std::cout << "crypto++\n";

	uint8_t buffer_cryptopp[bufferlen];
	fill_buffer(buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Blowfish>::Encryption encctx;
	    encctx.SetKey(enckey, 16);

	    encctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	compare_buffers(buffer_gcrypt, buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::Blowfish>::Decryption decctx;
	    decctx.SetKey(enckey, 16);

	    decctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	check_buffer(buffer_cryptopp, bufferlen);
    }
#endif

#if HAVE_OPENSSL
    {
	std::cout << "openssl\n";

	uint8_t buffer_openssl[bufferlen];
	fill_buffer(buffer_openssl, bufferlen);

	{
	    BF_KEY encctx;
	    BF_set_key(&encctx, 16, enckey);

	    for(unsigned int p = 0; p < bufferlen; p += BF_BLOCK)
		BF_ecb_encrypt(buffer_openssl + p, buffer_openssl + p, &encctx, BF_ENCRYPT);
	}

	compare_buffers(buffer_gcrypt, buffer_openssl, bufferlen);

	{
	    BF_KEY decctx;
	    BF_set_key(&decctx, 16, enckey);

	    for(unsigned int p = 0; p < bufferlen; p += BF_BLOCK)
		BF_ecb_encrypt(buffer_openssl + p, buffer_openssl + p, &decctx, BF_DECRYPT);
	}

	check_buffer(buffer_openssl, bufferlen);
    }
#endif

#if HAVE_LIBNETTLE
    {
	std::cout << "nettle\n";

	uint8_t buffer_nettle[bufferlen];
	fill_buffer(buffer_nettle, bufferlen);

	{
	    blowfish_ctx encctx;
	    blowfish_set_key(&encctx, 16, enckey);
	    blowfish_encrypt(&encctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	compare_buffers(buffer_gcrypt, buffer_nettle, bufferlen);

	{
	    blowfish_ctx decctx;
	    blowfish_set_key(&decctx, 16, enckey);
	    blowfish_decrypt(&decctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	check_buffer(buffer_nettle, bufferlen);
    }
#endif

#if HAVE_LIBTOMCRYPT
    {
	std::cout << "tomcrypt\n";

	uint8_t buffer_tomcrypt[bufferlen];
	fill_buffer(buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB encctx;
	    ecb_start(find_cipher("blowfish"), enckey, 16, 0, &encctx);
	    ecb_encrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &encctx);
	    ecb_done(&encctx);
	}

	compare_buffers(buffer_gcrypt, buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB decctx;
	    ecb_start(find_cipher("blowfish"), enckey, 16, 0, &decctx);
	    ecb_decrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &decctx);
	    ecb_done(&decctx);
	}

	check_buffer(buffer_tomcrypt, bufferlen);
    }
#endif

    {
	std::cout << "libgcrypt\n";

	gcry_cipher_hd_t decctx;
	gcry_cipher_open(&decctx, GCRY_CIPHER_BLOWFISH, GCRY_CIPHER_MODE_ECB, 0);
	gcry_cipher_setkey(decctx, enckey, 16);
	gcry_cipher_decrypt(decctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	gcry_cipher_close(decctx);
    }

    check_buffer(buffer_gcrypt, bufferlen);

#endif // HAVE_LIBGCRYPT
}

// *** Verify Triple DES Implementations

void verify_3des_ecb()
{
    std::cout << "Testing Triple DES implementations\n";

#if HAVE_LIBNETTLE
    // Nettle requires some parity fix of the key

    des_fix_parity(24, enckey, enckey);
#endif

#if HAVE_LIBGCRYPT
    // libgcrypt

    uint8_t buffer_gcrypt[bufferlen];
    fill_buffer(buffer_gcrypt, bufferlen);

    {
	gcry_cipher_hd_t encctx;
	gcry_cipher_open(&encctx, GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_ECB, 0);
	gcry_cipher_setkey(encctx, enckey, 24);
	gcry_cipher_encrypt(encctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	gcry_cipher_close(encctx);
    }

#if HAVE_LIBMCRYPT
    {
	std::cout << "libmcrypt\n";

	uint8_t buffer_mcrypt[bufferlen];
	fill_buffer(buffer_mcrypt, bufferlen);

	{
	    MCRYPT encctx = mcrypt_module_open(MCRYPT_3DES, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(encctx, enckey, 24, NULL);
	    mcrypt_generic(encctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(encctx);
	}

	compare_buffers(buffer_gcrypt, buffer_mcrypt, bufferlen);

	{
	    MCRYPT decctx = mcrypt_module_open(MCRYPT_3DES, NULL, MCRYPT_ECB, NULL);
	    mcrypt_generic_init(decctx, enckey, 24, NULL);
	    mdecrypt_generic(decctx, buffer_mcrypt, bufferlen);
	    mcrypt_generic_end(decctx);
	}

	check_buffer(buffer_mcrypt, bufferlen);
    }
#endif

#if HAVE_BOTAN
    {
	std::cout << "botan\n";

	uint8_t buffer_botan[bufferlen];
	fill_buffer(buffer_botan, bufferlen);

	{
	    Botan::TripleDES encctx;
	    encctx.set_key((Botan::byte*)enckey, 24);

	    for(unsigned int p = 0; p < bufferlen; p += encctx.BLOCK_SIZE)
		encctx.encrypt((Botan::byte*)buffer_botan + p);
	}

	compare_buffers(buffer_gcrypt, buffer_botan, bufferlen);

	{
	    Botan::TripleDES decctx;
	    decctx.set_key((Botan::byte*)enckey, 24);

	    for(unsigned int p = 0; p < bufferlen; p += decctx.BLOCK_SIZE)
		decctx.decrypt((Botan::byte*)buffer_botan + p);
	}

	check_buffer(buffer_botan, bufferlen);
    }
#endif

#if HAVE_CRYPTOPP
    {
	std::cout << "crypto++\n";

	uint8_t buffer_cryptopp[bufferlen];
	fill_buffer(buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::DES_EDE3>::Encryption encctx;
	    encctx.SetKey(enckey, 24);

	    encctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	compare_buffers(buffer_gcrypt, buffer_cryptopp, bufferlen);

	{
	    CryptoPP::ECB_Mode<CryptoPP::DES_EDE3>::Decryption decctx;
	    decctx.SetKey(enckey, 24);

	    decctx.ProcessData(buffer_cryptopp, buffer_cryptopp, bufferlen);
	}

	check_buffer(buffer_cryptopp, bufferlen);
    }
#endif

#if HAVE_OPENSSL
    {
	std::cout << "openssl\n";

	uint8_t buffer_openssl[bufferlen];
	fill_buffer(buffer_openssl, bufferlen);

	{
	    DES_key_schedule eks1, eks2, eks3;

	    DES_set_key((DES_cblock*)(enckey +  0), &eks1);
	    DES_set_key((DES_cblock*)(enckey +  8), &eks2);
	    DES_set_key((DES_cblock*)(enckey + 16), &eks3);

	    for(unsigned int p = 0; p < bufferlen; p += 8)
		DES_encrypt3((DES_LONG*)(buffer_openssl + p), &eks1, &eks2, &eks3);
	}

	compare_buffers(buffer_gcrypt, buffer_openssl, bufferlen);

	{
	    DES_key_schedule dks1, dks2, dks3;

	    DES_set_key((DES_cblock*)(enckey +  0), &dks1);
	    DES_set_key((DES_cblock*)(enckey +  8), &dks2);
	    DES_set_key((DES_cblock*)(enckey + 16), &dks3);

	    for(unsigned int p = 0; p < bufferlen; p += 8)
		DES_decrypt3((DES_LONG*)(buffer_openssl + p), &dks1, &dks2, &dks3);
	}

	check_buffer(buffer_openssl, bufferlen);
    }
#endif

#if HAVE_LIBNETTLE
    {
	std::cout << "nettle\n";

	uint8_t buffer_nettle[bufferlen];
	fill_buffer(buffer_nettle, bufferlen);

	{
	    des3_ctx encctx;
	    des3_set_key(&encctx, enckey);
	    des3_encrypt(&encctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	compare_buffers(buffer_gcrypt, buffer_nettle, bufferlen);

	{
	    des3_ctx decctx;
	    des3_set_key(&decctx, enckey);
	    des3_decrypt(&decctx, bufferlen, buffer_nettle, buffer_nettle);
	}

	check_buffer(buffer_nettle, bufferlen);
    }
#endif

#if HAVE_LIBTOMCRYPT
    {
	std::cout << "tomcrypt\n";

	uint8_t buffer_tomcrypt[bufferlen];
	fill_buffer(buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB encctx;
	    ecb_start(find_cipher("3des"), enckey, 24, 0, &encctx);
	    ecb_encrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &encctx);
	    ecb_done(&encctx);
	}

	compare_buffers(buffer_gcrypt, buffer_tomcrypt, bufferlen);

	{
	    symmetric_ECB decctx;
	    ecb_start(find_cipher("3des"), enckey, 24, 0, &decctx);
	    ecb_decrypt(buffer_tomcrypt, buffer_tomcrypt, bufferlen, &decctx);
	    ecb_done(&decctx);
	}

	check_buffer(buffer_tomcrypt, bufferlen);
    }
#endif

    {
	std::cout << "libgcrypt\n";

	gcry_cipher_hd_t decctx;
	gcry_cipher_open(&decctx, GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_ECB, 0);
	gcry_cipher_setkey(decctx, enckey, 24);
	gcry_cipher_decrypt(decctx, buffer_gcrypt, bufferlen, buffer_gcrypt, bufferlen);
	gcry_cipher_close(decctx);
    }

    check_buffer(buffer_gcrypt, bufferlen);

#endif // HAVE_LIBGCRYPT
}

int main()
{
    // Initialize all cryptographic libaries

#if HAVE_LIBGCRYPT
    gcry_check_version(GCRYPT_VERSION);
#endif

#if HAVE_BOTAN
    Botan::LibraryInitializer init;
#endif

#if HAVE_LIBTOMCRYPT
    register_cipher(&rijndael_desc);
    register_cipher(&twofish_desc);
    register_cipher(&blowfish_desc);
    register_cipher(&des3_desc);
#endif

    // Create (somewhat) random encryption key

    srand(time(NULL));

    for(unsigned int i = 0; i < sizeof(enckey); ++i)
	enckey[i] = rand();

    // Verify cipher implementations

    verify_rijndael_ecb();
    verify_serpent_ecb();
    verify_twofish_ecb();
    verify_blowfish_ecb();
    verify_3des_ecb();

    return 0;
}
