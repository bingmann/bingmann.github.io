// $Id: serpent.h 74 2008-02-25 23:38:09Z tb $

#ifndef SERPENT_H
#define SERPENT_H

#include <stdint.h>
#include <stdlib.h>

namespace SerpentBotan {

/**
 * Serpent encryption cipher state context to encrypt input data blocks in ECB
 * (Electronic codebook) mode.
 */
class EncryptECB
{
private:
    /// storage for the key schedule
    uint32_t	round_key[132];

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
 * Serpent encryption cipher state context to decrypt input data blocks in ECB
 * (Electronic codebook) mode.
 */
class DecryptECB
{
private:
    /// storage for the key schedule
    uint32_t	round_key[132];

public:

    /// Set the decryption key. The key must be 16, 24 or 32 bytes long.
    void set_key(const unsigned char* key, unsigned int keylen);

    /// Decrypt a block of 16 bytes using the current cipher state.
    void decrypt_block(const uint8_t src[16], uint8_t dst[16]) const;

    /// Decrypt a length of n*16 bytes. Len must be a multiple of 16 or this
    /// function will assert().
    void decrypt(const void* src, void* dst, size_t len) const;
};

/**
 * Serpent encryption cipher state context to encrypt input data blocks in CBC
 * (Cipher-block chaining) mode.
 */
class EncryptCBC
{
private:
    /// storage for the key schedule
    uint32_t	l_key[132];

    /// cbc initialisation vector
    uint8_t	l_cbciv[16];

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
 * Serpent encryption cipher state context to decrypt input data blocks in CBC
 * (Cipher-block chaining) mode.
 */
class DecryptCBC
{
private:
    /// storage for the key schedule
    uint32_t	l_key[132];

    /// cbc initialisation vector
    uint8_t	l_cbciv[16];

    /// temporary cbc block
    uint8_t	l_cbcivsave[16];

public:

    /// Set the decryption key. The key must be 16, 24 or 32 bytes long.
    void set_key(const unsigned char* key, unsigned int keylen);

    /// Set the initial cbc vector. The vector is always 16 bytes long.
    void set_cbciv(const uint8_t iv[16]);

    /// Decrypt a block of 16 bytes using the current cipher state.
    void decrypt_block(const uint8_t src[16], uint8_t dst[16]);

    /// Decrypt a length of n*16 bytes. Len must be a multiple of 16 or this
    /// function will assert().
    void decrypt(const void* src, void* dst, size_t len);
};

} // namespace SerpentBotan

#endif // SERPENT_H
