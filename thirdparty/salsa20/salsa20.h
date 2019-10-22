/*
BASED ON salsa20-ref.c version 20051118
original author: D. J. Bernstein
Public domain.
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct salsa20_ctx
{
    uint32_t m_input[16];
} salsa20_ctx_t;

/*
 * Initializes the context with the chosen key.
 *
 * ctx: The context to initialize.
 * key: A pointer to the key bytes.
 * keySize: The length of the key in bits. Has to be 128 or 256.
 */
void Salsa20_KeySetup(salsa20_ctx_t* ctx, const uint8_t* key, uint32_t keySize);

/*
 * Initializes or changes the IV of the context.
 * Can be called multiple times on the same context in order to encrypt/decrypt messages with the same key but different IV's.
 *
 * ctx: The context to initialize.
 * iv: A pointer to the IV bytes. Must be 64 bits long.
 */
void Salsa20_IVSetup(salsa20_ctx_t* ctx, const uint8_t* iv);

/*
 * Encrypts the specified amount of plain text and writes it to the cipher text buffer.
 * The cipher text buffer and the plain text buffer can be the same.
 *
 * ctx: The context that was previously initialized with a key and IV.
 * cipherText: A pointer to the cipher text buffer which must at least be msgLen bytes.
 * plainText: A pointer to the plain text buffer which must at least be msgLen bytes.
 * msgLen: The amount of bytes to encrypt.
 */
void Salsa20_Encrypt_Bytes(salsa20_ctx_t* ctx, const uint8_t* plainText, uint8_t* cipherText, uint32_t msgLen);

/*
 * Decrypts the specified amount of cipher text and writes it to the plain text buffer.
 * The cipher text buffer and the plain text buffer can be the same.
 * Internally just calls the encryption method.
 *
 * ctx: The context that was previously initialized with a key and IV.
 * cipherText: A pointer to the cipher text buffer which must at least be msgLen bytes.
 * plainText: A pointer to the plain text buffer which must at least be msgLen bytes.
 * msgLen: The amount of bytes to decrypt.
 */
void Salsa20_Decrypt_Bytes(salsa20_ctx_t* ctx, const uint8_t* cipherText, uint8_t* plainText, uint32_t msgLen);

#ifdef __cplusplus
}
#endif