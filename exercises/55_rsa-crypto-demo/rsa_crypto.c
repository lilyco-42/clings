/* 55_rsa-crypto-demo.c — RSA 公钥加密演示 (uint64 玩具版)
 *
 * 任务：实现 RSA 公钥加密的 8 个核心组件，完成一次完整的
 *       "素性判定 → 密钥生成 → 加密 → 解密 → 验证"流程。
 *
 * RSA 固定参数 (不可修改):
 *   p = 61, q = 53  (两个素数)
 *   n = p * q = 3233 (模数)
 *   φ(n) = (p-1)*(q-1) = 3120 (欧拉函数)
 *   e = 17 (公钥指数)
 *   m = 42 (明文消息，固定)
 *
 * 需实现的功能：
 *   TODO 1: gcd(a, b)             — 欧几里得算法求最大公约数
 *   TODO 2: ext_gcd(a, b, &x, &y)  — 扩展欧几里得算法，求 e 的模逆元 d
 *   TODO 3: fast_pow(base,exp,mod) — 快速模幂运算 (二分求幂)
 *   TODO 4: is_prime(num)          — Miller-Rabin 素性判定
 *   TODO 5: gen_keys(p,q,e,&n,&φ,&d) — 密钥生成，含 e 有效性验证
 *   TODO 6: encrypt(msg, e, n)     — RSA 加密：c = m^e mod n
 *   TODO 7: decrypt(cipher, d, n)  — RSA 解密：m' = c^d mod n
 *   TODO 8: main() 主流程          — 串联所有步骤并输出
 *
 * 知识点：素性判定、Miller-Rabin、非对称加密、欧拉函数、模逆元、
 *         扩展欧几里得、快速幂取模、密钥生成流程
 *
 * 验证：make test  →  ./rsa_crypto | diff - expected_output.txt
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/* ─── RSA 固定参数 ─── */
static const uint64_t p = 61;
static const uint64_t q = 53;
static const uint64_t n = 3233;   /* p * q */
static const uint64_t phi = 3120; /* (p-1)*(q-1) */
static const uint64_t e = 17;     /* 公钥指数 */
static const uint64_t m = 42;     /* 明文消息 */

/* ─── TODO 1: 最大公约数 (欧几里得算法) ─── */
static uint64_t gcd(uint64_t a, uint64_t b) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 2: 扩展欧几里得算法 ───
 *
 * 求整数 x, y 使得 a*x + b*y = gcd(a, b)
 * 将 x 存入 *px, y 存入 *py。
 * 返回 gcd(a, b)。
 *
 * 提示：递归实现。基准情况 b==0 时 {*px=1; *py=0; return a;}。
 *       递归调用 ext_gcd(b, a%b, &x1, &y1)，
 *       然后 *px = y1, *py = x1 - (a/b)*y1。
 */
static int64_t ext_gcd(int64_t a, int64_t b, int64_t *px, int64_t *py) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 3: 快速模幂运算 ───
 *
 * 计算 (base^exp) % mod，使用二分求幂法 (binary exponentiation)。
 * 时间复杂度 O(log exp)。
 *
 * 提示：result = 1; base = base % mod;
 *       while (exp > 0): 若 exp 最低位为 1 → result = (result*base)%mod
 *                        exp >>= 1; base = (base*base)%mod
 */
static uint64_t fast_pow(uint64_t base, uint64_t exp, uint64_t mod) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 4: Miller-Rabin 素性判定 ───
 *
 * 使用 Miller-Rabin 概率素性测试判定 num 是否为素数。
 * 使用见证人集合 {2, 3, 5, 7, 11} 可对所有 < 2^64 的数给出确定性结果。
 *
 * 算法步骤：
 *   1. 处理小情况：num < 2 → false; num ∈ {2,3} → true; num 偶数 → false
 *   2. 将 num-1 写成 d * 2^s 的形式，其中 d 为奇数
 *   3. 对每个见证人 a:
 *      a. 计算 x = a^d mod num (调用 fast_pow)
 *      b. 若 x == 1 或 x == num-1，此见证人通过，继续下一个
 *      c. 重复 s-1 次：x = x^2 mod num; 若 x == num-1 则跳出
 *      d. 若 x != num-1，返回 false (合数)
 *   4. 所有见证人通过，返回 true (素数)
 *
 * 提示：分解 n-1 的 2 的幂时使用 while ((d & 1) == 0) { d >>= 1; s++; }
 */
static int is_prime(uint64_t num) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 5: 密钥生成 ───
 *
 * 从两个素数生成 RSA 密钥对，验证 e 的有效性，输出生成过程。
 *
 * 步骤：
 *   1. 计算 n = p*q, φ = (p-1)*(q-1)
 *   2. 按格式输出 n, φ, e
 *   3. 验证 1 < e < φ 且 gcd(e, φ) == 1（调用 gcd），否则报错返回 1
 *   4. 调用 ext_gcd 计算 d = e⁻¹ mod φ
 *   5. 输出 ext_gcd 结果、方程、d、验证 e*d mod φ == 1
 *   6. 将 n, φ, d 通过 out_n/out_phi/out_d 传出
 *   7. 成功返回 0
 *
 * 输出格式参考 expected_output.txt Step 2 部分。
 * 使用 PRIu64 / PRId64 宏格式化 uint64_t / int64_t。
 */
static int gen_keys(uint64_t prime_p, uint64_t prime_q, uint64_t pub_e, uint64_t *out_n, uint64_t *out_phi,
                    int64_t *out_d) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 6: RSA 加密 ─── */
static uint64_t encrypt(uint64_t msg, uint64_t pub_exp, uint64_t modulus) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 7: RSA 解密 ─── */
static uint64_t decrypt(uint64_t cipher, uint64_t priv_exp, uint64_t modulus) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ─── TODO 8: 主流程 ───
 *
 * 按以下步骤输出 (格式须与 expected_output.txt 完全一致):
 *   Step 1: 打印 p, q 并用 is_prime 验证素性，打印 n, φ, e, m
 *   Step 2: 调用 gen_keys() 完成密钥生成与验证
 *   Step 3: 加密，打印 c = m^e mod n
 *   Step 4: 解密，打印 m' = c^d mod n
 *   Step 5: 验证 m' == m，打印 RSA works! ✓
 *
 * 使用 PRIu64 / PRId64 宏格式化 uint64_t / int64_t (来自 <inttypes.h>)。
 * is_prime 返回值：1=素数，0=合数，打印 "prime ✓" 或 "composite ✗"。
 */
int main(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}
