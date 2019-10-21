/*
BASED ON salsa20-ref.c version 20051118
original author: D. J. Bernstein
Public domain.
*/

#include "salsa20.h"
#include <assert.h>

#define U8V(v) ((uint8_t)(v) & 0xFFu)
#define U16V(v) ((uint16_t)(v) & 0xFFFFu)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFu)
#define U64V(v) ((uint64_t)(v) & 0xFFFFFFFFFFFFFFFFu)

#define ROTL8(v, n) \
  (U8V((v) << (n)) | ((v) >> (8 - (n))))

#define ROTL16(v, n) \
  (U16V((v) << (n)) | ((v) >> (16 - (n))))

#define ROTL32(v, n) \
  (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTL64(v, n) \
  (U64V((v) << (n)) | ((v) >> (64 - (n))))

#define SWAP32(v) \
  ((ROTL32(v,  8) & 0x00FF00FFu) | \
   (ROTL32(v, 24) & 0xFF00FF00u))

#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v),1))

#ifdef BIG_ENDIAN
#define U32TO8_LITTLE(output8, input32) (((uint32_t*)(output8))[0] = SWAP32(input32))
#define U8TO32_LITTLE(input8) (SWAP32(((uint32_t*)(input8))[0]))
#else
#define U32TO8_LITTLE(output8, input32) (((uint32_t*)(output8))[0] = (input32))
#define U8TO32_LITTLE(input8) (((uint32_t*)(input8))[0])
#endif

static void Salsa20_WordToByte(uint8_t output[64], const uint32_t input[16])
{
    uint32_t x[16];
    int i;

    for (i = 0; i < 16; ++i) x[i] = input[i];
    for (i = 20; i > 0; i -= 2)
    {
        x[ 4] ^= ROTL32(PLUS(x[ 0], x[12]), 7);
        x[ 8] ^= ROTL32(PLUS(x[ 4], x[ 0]), 9);
        x[12] ^= ROTL32(PLUS(x[ 8], x[ 4]), 13);
        x[ 0] ^= ROTL32(PLUS(x[12], x[ 8]), 18);
        x[ 9] ^= ROTL32(PLUS(x[ 5], x[ 1]), 7);
        x[13] ^= ROTL32(PLUS(x[ 9], x[ 5]), 9);
        x[ 1] ^= ROTL32(PLUS(x[13], x[ 9]), 13);
        x[ 5] ^= ROTL32(PLUS(x[ 1], x[13]), 18);
        x[14] ^= ROTL32(PLUS(x[10], x[ 6]), 7);
        x[ 2] ^= ROTL32(PLUS(x[14], x[10]), 9);
        x[ 6] ^= ROTL32(PLUS(x[ 2], x[14]), 13);
        x[10] ^= ROTL32(PLUS(x[ 6], x[ 2]), 18);
        x[ 3] ^= ROTL32(PLUS(x[15], x[11]), 7);
        x[ 7] ^= ROTL32(PLUS(x[ 3], x[15]), 9);
        x[11] ^= ROTL32(PLUS(x[ 7], x[ 3]), 13);
        x[15] ^= ROTL32(PLUS(x[11], x[ 7]), 18);
        x[ 1] ^= ROTL32(PLUS(x[ 0], x[ 3]), 7);
        x[ 2] ^= ROTL32(PLUS(x[ 1], x[ 0]), 9);
        x[ 3] ^= ROTL32(PLUS(x[ 2], x[ 1]), 13);
        x[ 0] ^= ROTL32(PLUS(x[ 3], x[ 2]), 18);
        x[ 6] ^= ROTL32(PLUS(x[ 5], x[ 4]), 7);
        x[ 7] ^= ROTL32(PLUS(x[ 6], x[ 5]), 9);
        x[ 4] ^= ROTL32(PLUS(x[ 7], x[ 6]), 13);
        x[ 5] ^= ROTL32(PLUS(x[ 4], x[ 7]), 18);
        x[11] ^= ROTL32(PLUS(x[10], x[ 9]), 7);
        x[ 8] ^= ROTL32(PLUS(x[11], x[10]), 9);
        x[ 9] ^= ROTL32(PLUS(x[ 8], x[11]), 13);
        x[10] ^= ROTL32(PLUS(x[ 9], x[ 8]), 18);
        x[12] ^= ROTL32(PLUS(x[15], x[14]), 7);
        x[13] ^= ROTL32(PLUS(x[12], x[15]), 9);
        x[14] ^= ROTL32(PLUS(x[13], x[12]), 13);
        x[15] ^= ROTL32(PLUS(x[14], x[13]), 18);
    }
    for (i = 0; i < 16; ++i) x[i] = PLUS(x[i], input[i]);
    for (i = 0; i < 16; ++i) U32TO8_LITTLE(output + 4 * i, x[i]);
}

static const char* sigma = "expand 32-byte k";
static const char* tau = "expand 16-byte k";

void Salsa20_KeySetup(salsa20_ctx_t* ctx, const uint8_t* key, const uint32_t keySize)
{
    const char* constants;

    assert(keySize == 256 || keySize == 128);

    ctx->m_input[1] = U8TO32_LITTLE(key + 0);
    ctx->m_input[2] = U8TO32_LITTLE(key + 4);
    ctx->m_input[3] = U8TO32_LITTLE(key + 8);
    ctx->m_input[4] = U8TO32_LITTLE(key + 12);
    if (keySize == 256)
    {
        /* recommended */
        key += 16;
        constants = sigma;
    }
    else
    {
        /* keySize == 128 */
        constants = tau;
    }
    ctx->m_input[11] = U8TO32_LITTLE(key + 0);
    ctx->m_input[12] = U8TO32_LITTLE(key + 4);
    ctx->m_input[13] = U8TO32_LITTLE(key + 8);
    ctx->m_input[14] = U8TO32_LITTLE(key + 12);
    ctx->m_input[0] = U8TO32_LITTLE(constants + 0);
    ctx->m_input[5] = U8TO32_LITTLE(constants + 4);
    ctx->m_input[10] = U8TO32_LITTLE(constants + 8);
    ctx->m_input[15] = U8TO32_LITTLE(constants + 12);
}

void Salsa20_IVSetup(salsa20_ctx_t* ctx, const uint8_t* iv)
{
    ctx->m_input[6] = U8TO32_LITTLE(iv + 0);
    ctx->m_input[7] = U8TO32_LITTLE(iv + 4);
    ctx->m_input[8] = 0;
    ctx->m_input[9] = 0;
}

void Salsa20_Encrypt_Bytes(salsa20_ctx_t* ctx, const uint8_t* plainText, uint8_t* cipherText, uint32_t msgLen)
{
    uint8_t output[64];
    unsigned int i;

    if (!msgLen) return;
    for (;;)
    {
        Salsa20_WordToByte(output, ctx->m_input);
        ctx->m_input[8] = PLUSONE(ctx->m_input[8]);
        if (!ctx->m_input[8])
        {
            ctx->m_input[9] = PLUSONE(ctx->m_input[9]);
            /* stopping at 2^70 bytes per nonce is user's responsibility */
        }
        if (msgLen <= 64)
        {
            for (i = 0; i < msgLen; ++i) cipherText[i] = plainText[i] ^ output[i];
            return;
        }
        for (i = 0; i < 64; ++i) cipherText[i] = plainText[i] ^ output[i];
        msgLen -= 64;
        cipherText += 64;
        plainText += 64;
    }
}

void Salsa20_Decrypt_Bytes(salsa20_ctx_t* ctx, const uint8_t* cipherText, uint8_t* plainText, const uint32_t msgLen)
{
    Salsa20_Encrypt_Bytes(ctx, cipherText, plainText, msgLen);
}