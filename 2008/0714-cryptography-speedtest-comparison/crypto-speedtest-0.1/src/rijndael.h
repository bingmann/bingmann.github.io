// $Id: rijndael.h 65 2008-02-25 13:16:37Z tb $

#ifndef RIJNDAEL_H
#define RIJNDAEL_H

#include <inttypes.h>
#include <stdlib.h>

/**
 * AES state context to encrypt input data blocks in ECB mode.
 */
class RijndaelEncryptECB
{
protected:

    /// key-length-dependent number of rounds
    int		nr_;

    /// encrypt key schedule
    uint32_t	ek_[60];

public:    

    /// Set the encryption key. The key must be 16, 24 or 32 bytes long.
    void set_key(const unsigned char* key, unsigned int keylen);

    /// Encrypt a block of 16 bytes using the current cipher state.
    void encrypt_block(const uint8_t src[16], uint8_t dst[16]) const;

    /// Encrypt a length of n*16 bytes. Len must be a multiple of 16 or this
    /// function will assert().
    void encrypt(const void* src, void* dst, size_t len) const;
};

/**
 * AES state context to decrypt input data blocks in ECB mode.
 */
class RijndaelDecryptECB
{
protected:

    /// key-length-dependent number of rounds
    int		nr_;

    /// decrypt key schedule
    uint32_t	dk_[60];

public:

    /// Set the encryption key. The key must be 16, 24 or 32 bytes long.
    void set_key(const unsigned char* key, unsigned int keylen);

    /// Decrypt a block of 16 bytes using the current cipher state.
    void decrypt_block(const uint8_t src[16], uint8_t dst[16]) const;

    /// Decrypt a length of n*16 bytes. Len must be a multiple of 16 or this
    /// function will assert().
    void decrypt(const void* src, void* dst, size_t len) const;
};

/**
 * AES state context to encrypt input data blocks in CBC mode.
 */
class RijndaelEncryptCBC
{
protected:

    /// key-length-dependent number of rounds
    int		nr_;

    /// encrypt key schedule
    uint32_t	ek_[60];

    /// cbc initialisation vector
    uint8_t	cbciv_[16];

public:    

    /// Set the encryption key. The key must be 16, 24 or 32 bytes long.
    void set_key(const unsigned char* key, unsigned int keylen);

    /// Set the initial cbc vector. The vector is always 16 bytes long.
    void set_cbciv(const uint8_t iv[16]);

    /// Encrypt a block of 16 bytes using the current cipher state.
    void encrypt_block(const uint8_t src[16], uint8_t dst[16]);

    /// Encrypt a length of n*16 bytes. Len must be a multiple of 16 or this
    /// function will assert().
    void encrypt(const void* src, void* dst, size_t len);
};

/**
 * AES state context to decrypt input data blocks in CBC mode.
 */
class RijndaelDecryptCBC
{
protected:

    /// key-length-dependent number of rounds
    int		nr_;

    /// decrypt key schedule
    uint32_t	dk_[60];

    /// cbc initialisation vector
    uint8_t	cbciv_[16];

    /// temporary cbc block
    uint8_t	cbcivsave_[16];

public:

    /// Set the encryption key. The key must be 16, 24 or 32 bytes long.
    void set_key(const unsigned char* key, unsigned int keylen);

    /// Set the initial cbc vector. The vector is always 16 bytes long.
    void set_cbciv(const uint8_t iv[16]);

    /// Decrypt a block of 16 bytes using the current cipher state.
    void decrypt_block(const uint8_t src[16], uint8_t dst[16]);

    /// Decrypt a length of n*16 bytes. Len must be a multiple of 16 or this
    /// function will assert().
    void decrypt(const void* src, void* dst, size_t len);
};

#endif // RIJNDAEL_H
