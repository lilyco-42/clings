/* 71_simple-proof-of-work — Simplified Proof-of-Work Blockchain with SHA-256
 *
 * 任务：实现简化但完整的 SHA-256 hash，并用它构建 3 区块工作量证明区块链。
 *
 *       需要实现的函数（7 个 TODO）：
 *         sha256_init()   — 用标准 IV 初始化 SHA-256 上下文
 *         sha256_update() — 喂入数据，满 64 字节时调用 sha256_transform
 *         sha256_final()  — 消息填充、处理最后一块、输出 32 字节摘要
 *         sha256_string() — 便捷函数：hash 字符串，返回 64 字符 hex
 *         mine_block()    — PoW 挖矿：递增 nonce 直到 hash 满足目标
 *         print_block()   — 打印单个区块的完整信息
 *         main()          — 构建 3 个区块的链并验证
 *
 *       已提供的骨架（无需修改）：
 *         - SHA256_IV[8]   — 初始哈希值 (前 8 个质数平方根的小数部分)
 *         - SHA256_K[64]   — 64 轮常量 (前 64 个质数立方根的小数部分)
 *         - SHA256_CTX     — 哈希上下文结构体
 *         - rotr32()       — 32 位循环右移
 *         - sha256_transform() — 64 轮压缩函数（完整实现）
 *         - Block 结构体、block_data[]、hash_meets_target()、build_block_input()
 *
 *       区块数据：
 *         Block 0: "Genesis"           (prev_hash = 64 个 '0')
 *         Block 1: "Alice pays Bob 10" (prev_hash = block 0 的 hash)
 *         Block 2: "Bob pays Carol 5"  (prev_hash = block 1 的 hash)
 *
 *       难度目标：SHA-256 hash 的前 4 个 hex 字符必须为 "0000"
 *
 * 知识点：
 *   - SHA-256 消息填充 (0x80 + 零填充 + 64 位大端长度)
 *   - SHA-256 64 轮压缩与 Merkle-Damgård 结构
 *   - 工作量证明 (Proof-of-Work) 共识机制
 *   - 区块链数据结构与 hash 链
 *
 * 验证：
 *   make && ./pow_chain | diff - expected_output.txt
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ─── SHA-256 常量（已提供）─────────────────────────────────────────────── */

/* 初始哈希值 H(0) — 前 8 个质数 (2..19) 平方根小数部分的前 32 位 */
static const uint32_t SHA256_IV[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

/* 轮常量 K — 前 64 个质数 (2..311) 立方根小数部分的前 32 位 */
static const uint32_t SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/* ─── SHA-256 上下文（已提供）───────────────────────────────────────────── */
typedef struct {
    uint32_t state[8]; /* 当前哈希状态 (H)               */
    uint64_t bitlen;   /* 已处理的总位数                 */
    uint8_t block[64]; /* 当前 512 位消息块              */
    int block_idx;     /* block 中已累积的字节数         */
} SHA256_CTX;

/* ─── 辅助函数：32 位循环右移（已提供）──────────────────────────────────── */
static uint32_t rotr32(uint32_t x, unsigned int n) { return (x >> n) | (x << (32 - n)); }

/* ─── sha256_transform — 处理一个 512 位块（已提供，无需修改）────────────
 *
 * 这是 SHA-256 的核心 64 轮压缩函数。
 * 从 sha256_update（累积满 64 字节时）和 sha256_final（填充后）中调用。
 *
 * 工作流程：
 *   1. 从 ctx->block 展开消息调度表 W[0..63]
 *   2. 用 ctx->state 初始化 8 个工作变量 a..h
 *   3. 执行 64 轮位混合 (Σ0, Σ1, Ch, Maj, +K, +W)
 *   4. 将结果加回 ctx->state (Merkle-Damgård 的 Davies-Meyer 步骤)
 */
static void sha256_transform(SHA256_CTX *ctx) {
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    int i;

    /* 准备消息调度表 W[0..63] */
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

    /* 从当前哈希状态初始化工作变量 */
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    /* 64 轮 */
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

    /* 将压缩结果加回当前哈希状态 */
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;

    /* 安全清零 block */
    for (i = 0; i < 64; i++) ctx->block[i] = 0;
}

/* ─── TODO 1: sha256_init ─────────────────────────────────────────────────
 *
 * static void sha256_init(SHA256_CTX *ctx)
 *
 * 初始化 SHA-256 上下文：
 *   1. 将 ctx->state[0..7] 复制自 SHA256_IV[0..7]
 *   2. ctx->bitlen = 0
 *   3. ctx->block_idx = 0
 *
 * 提示：用 for 循环复制 8 个 state 值。
 */
#error TODO 1: Implement sha256_init(ctx).

/* ─── TODO 2: sha256_update ───────────────────────────────────────────────
 *
 * static void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len)
 *
 * 喂入数据到哈希上下文：
 *   1. 遍历 data[0..len-1] 的每个字节：
 *      a. 存入 ctx->block[ctx->block_idx]，block_idx 加 1
 *      b. ctx->bitlen 加 8 (每个字节 8 位)
 *      c. 如果 block_idx == 64 (满块)：
 *         - 调用 sha256_transform(ctx) 压缩当前块
 *         - block_idx 重置为 0
 *
 * 提示：这是 Merkle-Damgård 结构的"吸收"阶段。
 */
#error TODO 2: Implement sha256_update(ctx, data, len).

/* ─── TODO 3: sha256_final ────────────────────────────────────────────────
 *
 * static void sha256_final(SHA256_CTX *ctx, uint8_t digest[32])
 *
 * SHA-256 填充并产生最终 32 字节摘要（FIPS 180-4 第 5.1.1 节）：
 *
 *   1. 记录 total_bits = ctx->bitlen
 *   2. 追加 0x80 (即二进制 '1' 后跟 '0' 的起始字节)
 *      ctx->block[ctx->block_idx++] = 0x80;
 *   3. 如果 block_idx > 56（放不下 8 字节长度字段）：
 *      a. 用 0x00 填充至 64 字节
 *      b. 调用 sha256_transform(ctx)
 *      c. block_idx = 0
 *   4. 用 0x00 填充至 block_idx == 56
 *   5. 在位置 56..63 以大端序写入 64 位 total_bits
 *      （从高字节到低字节：i 从 7 递减到 0）
 *   6. 调用 sha256_transform(ctx) 处理最后一个块
 *   7. 输出摘要：将 ctx->state[0..7] 以大端序写入 digest[0..31]
 *      每个 state[i] 占 4 字节：digest[i*4+0] = state[i] >> 24, 等等。
 *
 * 关键：消息长度总是 64 位大端序，填充总是在 block 的字节 56..63。
 */
#error TODO 3: Implement sha256_final(ctx, digest).

/* ─── TODO 4: sha256_string ───────────────────────────────────────────────
 *
 * static void sha256_string(const char *input, char hex_out[65])
 *
 * 便捷函数：对字符串做 SHA-256，输出 64 字符十六进制小写 + '\0'：
 *   1. 声明 SHA256_CTX ctx 和 uint8_t digest[32]
 *   2. sha256_init(&ctx)
 *   3. sha256_update(&ctx, (const uint8_t *)input, strlen(input))
 *   4. sha256_final(&ctx, digest)
 *   5. 对 i = 0..31：用 sprintf(hex_out + i*2, "%02x", digest[i])
 *   6. hex_out[64] = '\0'
 *
 * 提示：%02x 输出 2 位小写十六进制，不足补零。
 */
#error TODO 4: Implement sha256_string(input, hex_out).

/* ─── Block 结构体（已提供）──────────────────────────────────────────────── */
typedef struct {
    char prev_hash[65]; /* 64 字符 hex + '\0'          */
    const char *data;   /* 区块数据（交易内容）         */
    uint64_t nonce;     /* 工作量证明随机数             */
    char hash[65];      /* 64 字符 hex + '\0'          */
    uint64_t attempts;  /* 挖矿尝试次数                 */
} Block;

/* ─── 区块数据数组（已提供）──────────────────────────────────────────────── */
const char *block_data[] = {"Genesis", "Alice pays Bob 10", "Bob pays Carol 5"};

/* ─── 难度检查：hash 前 4 字符必须为 "0000"（已提供）────────────────────── */
static int hash_meets_target(const char hash_hex[65]) {
    return hash_hex[0] == '0' && hash_hex[1] == '0' && hash_hex[2] == '0' && hash_hex[3] == '0';
}

/* ─── 构造区块的 hash 输入字符串（已提供）───────────────────────────────── */
static void build_block_input(const Block *block, uint64_t nonce, char *out, size_t out_size) {
    snprintf(out, out_size, "%s%s%lu", block->prev_hash, block->data, (unsigned long)nonce);
}

/* ─── TODO 5: mine_block — 工作量证明挖矿 ─────────────────────────────────
 *
 * static uint64_t mine_block(Block *block)
 *
 * 工作量证明挖矿：
 *   1. nonce = 0, attempts = 0
 *   2. 无限循环：
 *      a. 调用 build_block_input(block, nonce, input, sizeof(input))
 *         构造待 hash 的字符串（声明 char input[512], char hex[65]）
 *      b. sha256_string(input, hex) 计算 SHA-256 hash
 *      c. attempts++
 *      d. 如果 hash_meets_target(hex) 为真（前 4 字符 "0000"）：
 *         - block->nonce = nonce
 *         - strcpy(block->hash, hex)
 *         - block->attempts = attempts
 *         - return attempts
 *      e. 否则 nonce++ 继续尝试
 *
 * 注意：nonce 使用 uint64_t 类型，挖矿可能需要数万次尝试。
 *       期望尝试次数约 2^16 = 65536 次（前 4 hex = 16 位为零）。
 */
#error TODO 5: Implement mine_block(block).

/* ─── TODO 6: print_block — 打印单个区块 ──────────────────────────────────
 *
 * static void print_block(int index, const Block *block)
 *
 * 打印区块信息，格式如下（严格匹配 expected_output.txt）：
 *
 *   Block <index>:
 *     prev_hash: <64 字符 hex>
 *     data:      <数据字符串>
 *     nonce:     <nonce 值 (十进制)>
 *     hash:      <64 字符 hex>
 *     attempts:  <尝试次数>
 *
 * 每行缩进 2 个空格。nonce 和 attempts 用 %lu 格式。
 * prev_hash 和 hash 用 %s 格式（已经是 hex 字符串）。
 */
#error TODO 6: Implement print_block(index, block).

/* ─── TODO 7: main — 构建 3 区块链并验证 ──────────────────────────────────
 *
 * int main(void)
 *
 * 主流程：
 *   1. 声明 Block chain[3] 数组
 *   2. 声明 uint64_t total_attempts = 0
 *   3. 打印标题：
 *        "=== Simple Proof-of-Work Blockchain (SHA-256) ===\n"
 *        "Difficulty: hash prefix must be \"0000\"\n\n"
 *   4. 对每个区块 i = 0, 1, 2：
 *      a. 设置 prev_hash：
 *         - i==0 时：memset(chain[i].prev_hash, '0', 64);
 *                    chain[i].prev_hash[64] = '\0';
 *         - i>0 时：strcpy(chain[i].prev_hash, chain[i-1].hash);
 *      b. 设置 chain[i].data = block_data[i]
 *      c. 打印 "Mining block %d: \"%s\"...\n"
 *      d. 调用 mine_block(&chain[i])
 *      e. total_attempts += chain[i].attempts
 *      f. 调用 print_block(i, &chain[i])
 *      g. 打印 "\n"
 *   5. 打印摘要：
 *        "=== Chain Summary ===\n"
 *        "Total blocks:     3\n"
 *        "Total attempts:   %lu\n"
 *        "Average attempts: %.1f\n"
 *   6. 验证链（声明 int valid = 1）：
 *        "\n=== Chain Verification ===\n"
 *      对每个区块 i：
 *      - 用 build_block_input + sha256_string 重新计算 hash
 *      - 打印 "Block %d: computed=%s  stored=%s"
 *      - 如果 computed == stored 且 hash_meets_target：打印 "  [VALID]\n"
 *        否则打印 "  [INVALID]\n" 并设 valid = 0
 *      - 对 i>0：检查 chain[i].prev_hash == chain[i-1].hash
 *        匹配打印 "  Chain link %d->%d: %s  [OK]\n"
 *        不匹配打印不匹配信息并设 valid = 0
 *   7. 打印 "\nChain integrity: %s\n" (VALID 或 INVALID)
 *   8. return 0
 */
#error TODO 7: Implement main() — build the 3-block chain and verify.

int main(void) {
#error TODO 7: Complete main() — declare chain, mine 3 blocks, verify integrity.
    return 0;
}
