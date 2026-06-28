/* 71_simple-proof-of-work — Simplified Proof-of-Work Blockchain with SHA-256
 *
 * Task: Implement a simplified but complete SHA-256 hash function and use it
 *       to build a 3-block proof-of-work blockchain.
 *
 *       Functions to implement:
 *         sha256_init()   — initialize SHA-256 context with standard IV
 *         sha256_update() — feed data into the hash context
 *         sha256_final()  — pad, process final block, produce 256-bit digest
 *         sha256_string() — convenience: hash a string, return 64 hex chars
 *         mine_block()    — PoW mining: try nonces until hash meets target
 *         print_block()   — display a single block's info
 *         main()          — build 3-block chain and verify
 *
 *       SHA-256 is provided in skeleton form:
 *         - IV (8 × 32-bit words) and K table (64 × 32-bit words) are given
 *         - sha256_transform() — the 64-round compression — is fully provided
 *         - You fill in init/update/final to complete the Merkle-Damgård loop
 *
 * Block data:
 *   Block 0: "Genesis"           (prev_hash = 64 zeros)
 *   Block 1: "Alice pays Bob 10" (prev_hash = block 0's hash)
 *   Block 2: "Bob pays Carol 5"  (prev_hash = block 1's hash)
 *
 * Difficulty target: first 4 hex chars of SHA-256 hash must be "0000"
 *
 * Knowledge points:
 *   - SHA-256 message padding (0x80 + zeros + 64-bit big-endian length)
 *   - SHA-256 64-round compression with Merkle-Damgård construction
 *   - Proof-of-Work consensus via hash target
 *   - Blockchain data structure and chaining
 *
 * Verification:
 *   make && ./pow_chain | diff - expected_output.txt
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ─── SHA-256 Constants (provided) ───────────────────────────────────────── */

/* Initial hash values H(0) — first 32 bits of fractional parts of
 * the square roots of the first 8 primes (2..19) */
static const uint32_t SHA256_IV[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

/* Round constants K — first 32 bits of fractional parts of
 * the cube roots of the first 64 primes (2..311) */
static const uint32_t SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/* ─── SHA-256 Context (provided) ─────────────────────────────────────────── */
typedef struct {
    uint32_t state[8]; /* current hash state (H)         */
    uint64_t bitlen;   /* total bits processed so far    */
    uint8_t block[64]; /* current 512-bit message block  */
    int block_idx;     /* how many bytes in block so far */
} SHA256_CTX;

/* ─── Helper: right rotation (provided) ──────────────────────────────────── */
static uint32_t rotr32(uint32_t x, unsigned int n) { return (x >> n) | (x << (32 - n)); }

/* ─── sha256_transform — process one 512-bit block (PROVIDED) ──────────────
 *
 * This is the core 64-round SHA-256 compression function.
 * You do NOT need to modify this.  Call it from sha256_update (when a full
 * 64-byte block is accumulated) and from sha256_final (after padding).
 *
 * It reads ctx->block, expands it into the W[0..63] message schedule,
 * performs 64 rounds of bit-mixing on the working variables, and
 * adds the result back into ctx->state.
 */
static void sha256_transform(SHA256_CTX *ctx) {
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    int i;

    /* Prepare message schedule W[0..63] */
    for (i = 0; i < 16; i++) {
        int p = i * 4;
        w[i] = ((uint32_t)ctx->block[p] << 24) | ((uint32_t)ctx->block[p + 1] << 16) |
               ((uint32_t)ctx->block[p + 2] << 8) | ((uint32_t)ctx->block[p + 3]);
    }
    for (i = 16; i < 64; i++) {
        uint32_t s0 = rotr32(w[i - 15], 7) ^ rotr32(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = rotr32(w[i - 2], 17) ^ rotr32(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    /* Initialize working variables from current hash state */
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    /* 64 rounds */
    for (i = 0; i < 64; i++) {
        uint32_t S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t temp1 = h + S1 + ch + SHA256_K[i] + w[i];
        uint32_t S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    /* Add compressed block to current hash state */
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;

    /* Zeroise the block for safety */
    for (i = 0; i < 64; i++) ctx->block[i] = 0;
}

/* ─── sha256_init — initialize SHA-256 context ───────────────────────────── */
static void sha256_init(SHA256_CTX *ctx) {
    int i;
    for (i = 0; i < 8; i++) {
        ctx->state[i] = SHA256_IV[i];
    }
    ctx->bitlen = 0;
    ctx->block_idx = 0;
}

/* ─── sha256_update — feed data into the hash ────────────────────────────── */
static void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        ctx->block[ctx->block_idx++] = data[i];
        ctx->bitlen += 8;
        if (ctx->block_idx == 64) {
            sha256_transform(ctx);
            ctx->block_idx = 0;
        }
    }
}

/* ─── sha256_final — pad and produce final 32-byte digest ────────────────── */
static void sha256_final(SHA256_CTX *ctx, uint8_t digest[32]) {
    uint64_t total_bits = ctx->bitlen;
    int i;

    /* Append 0x80 (the '1' bit) */
    ctx->block[ctx->block_idx++] = 0x80;

    /* If not enough room for the length field, pad with zeros and transform */
    if (ctx->block_idx > 56) {
        while (ctx->block_idx < 64) {
            ctx->block[ctx->block_idx++] = 0;
        }
        sha256_transform(ctx);
        ctx->block_idx = 0;
    }

    /* Pad with zeros until position 56 */
    while (ctx->block_idx < 56) {
        ctx->block[ctx->block_idx++] = 0;
    }

    /* Append 64-bit big-endian length */
    for (i = 7; i >= 0; i--) {
        ctx->block[56 + i] = (uint8_t)(total_bits & 0xFF);
        total_bits >>= 8;
    }

    /* Final transform */
    sha256_transform(ctx);

    /* Output digest in big-endian */
    for (i = 0; i < 8; i++) {
        digest[i * 4 + 0] = (uint8_t)(ctx->state[i] >> 24);
        digest[i * 4 + 1] = (uint8_t)(ctx->state[i] >> 16);
        digest[i * 4 + 2] = (uint8_t)(ctx->state[i] >> 8);
        digest[i * 4 + 3] = (uint8_t)(ctx->state[i]);
    }
}

/* ─── sha256_string — hash a string, return 64 hex chars + null ──────────── */
static void sha256_string(const char *input, char hex_out[65]) {
    SHA256_CTX ctx;
    uint8_t digest[32];
    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t *)input, strlen(input));
    sha256_final(&ctx, digest);
    for (int i = 0; i < 32; i++) {
        sprintf(hex_out + i * 2, "%02x", digest[i]);
    }
    hex_out[64] = '\0';
}

/* ─── Block structure (provided) ─────────────────────────────────────────── */
typedef struct {
    char prev_hash[65]; /* 64 hex chars + null          */
    const char *data;   /* block data (transaction)     */
    uint64_t nonce;     /* proof-of-work nonce           */
    char hash[65];      /* 64 hex chars + null          */
    uint64_t attempts;  /* number of mining attempts    */
} Block;

/* ─── Data for the 3 blocks (provided) ───────────────────────────────────── */
const char *block_data[] = {"Genesis", "Alice pays Bob 10", "Bob pays Carol 5"};

/* ─── Difficulty: hash must start with "0000" (provided) ─────────────────── */
static int hash_meets_target(const char hash_hex[65]) {
    return hash_hex[0] == '0' && hash_hex[1] == '0' && hash_hex[2] == '0' && hash_hex[3] == '0';
}

/* ─── Build the string to hash for a block (provided) ────────────────────── */
static void build_block_input(const Block *block, uint64_t nonce, char *out, size_t out_size) {
    snprintf(out, out_size, "%s%s%lu", block->prev_hash, block->data, (unsigned long)nonce);
}

/* ─── mine_block — proof-of-work mining ──────────────────────────────────── */
static uint64_t mine_block(Block *block) {
    uint64_t nonce = 0;
    char input[512];
    char hex[65];
    uint64_t attempts = 0;

    while (1) {
        build_block_input(block, nonce, input, sizeof(input));
        sha256_string(input, hex);
        attempts++;

        if (hash_meets_target(hex)) {
            block->nonce = nonce;
            strcpy(block->hash, hex);
            block->attempts = attempts;
            return attempts;
        }

        nonce++;
    }
}

/* ─── print_block — display a single block ───────────────────────────────── */
static void print_block(int index, const Block *block) {
    printf("Block %d:\n", index);
    printf("  prev_hash: %s\n", block->prev_hash);
    printf("  data:      %s\n", block->data);
    printf("  nonce:     %lu\n", (unsigned long)block->nonce);
    printf("  hash:      %s\n", block->hash);
    printf("  attempts:  %lu\n", (unsigned long)block->attempts);
}

/* ─── main — build the 3-block chain and verify ──────────────────────────── */
int main(void) {
    Block chain[3];
    uint64_t total_attempts = 0;
    int i, valid = 1;

    printf("=== Simple Proof-of-Work Blockchain (SHA-256) ===\n");
    printf("Difficulty: hash prefix must be \"0000\"\n\n");

    /* Mine each block */
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            memset(chain[i].prev_hash, '0', 64);
            chain[i].prev_hash[64] = '\0';
        } else {
            strcpy(chain[i].prev_hash, chain[i - 1].hash);
        }
        chain[i].data = block_data[i];

        printf("Mining block %d: \"%s\"...\n", i, chain[i].data);
        mine_block(&chain[i]);
        total_attempts += chain[i].attempts;
        print_block(i, &chain[i]);
        printf("\n");
    }

    /* Summary */
    printf("=== Chain Summary ===\n");
    printf("Total blocks:     3\n");
    printf("Total attempts:   %lu\n", (unsigned long)total_attempts);
    printf("Average attempts: %.1f\n", total_attempts / 3.0);

    /* Verification */
    printf("\n=== Chain Verification ===\n");
    for (i = 0; i < 3; i++) {
        char input[512];
        char computed[65];
        build_block_input(&chain[i], chain[i].nonce, input, sizeof(input));
        sha256_string(input, computed);

        printf("Block %d: computed=%s  stored=%s", i, computed, chain[i].hash);
        if (strcmp(computed, chain[i].hash) == 0 && hash_meets_target(computed)) {
            printf("  [VALID]\n");
        } else {
            printf("  [INVALID]\n");
            valid = 0;
        }

        if (i > 0) {
            if (strcmp(chain[i].prev_hash, chain[i - 1].hash) == 0) {
                printf("  Chain link %d->%d: %s  [OK]\n", i - 1, i, chain[i].prev_hash);
            } else {
                printf("  Chain link %d->%d: BROKEN! (prev=%s, expected=%s)\n", i - 1, i, chain[i].prev_hash,
                       chain[i - 1].hash);
                valid = 0;
            }
        }
    }
    printf("\nChain integrity: %s\n", valid ? "VALID" : "INVALID");

    return 0;
}
