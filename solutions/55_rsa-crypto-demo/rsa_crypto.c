/* 55_rsa-crypto-demo — RSA Public-Key Crypto Demo (uint64 toy version, solution)
 *
 * RSA parameters (fixed):
 *   p = 61, q = 53  →  n = p*q = 3233,  φ(n) = (p-1)*(q-1) = 3120
 *   e = 17 (public exponent, verified via gcd)
 *   d = 2753 (private exponent, computed via extended Euclidean: e*d ≡ 1 mod φ)
 *   m = 42 (fixed plaintext message)
 *
 * The program demonstrates:
 *   1. Miller-Rabin primality test to verify p, q are prime
 *   2. Key generation with gcd validation of e
 *   3. Extended Euclidean algorithm to compute d
 *   4. Fast modular exponentiation for encryption/decryption
 *   5. c = m^e mod n  (encrypt with public key)
 *   6. m' = c^d mod n (decrypt with private key)
 *   7. Verify m' == m
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/* ─── RSA fixed parameters ─── */
static const uint64_t p = 61;
static const uint64_t q = 53;
static const uint64_t n = 3233;   /* p * q */
static const uint64_t phi = 3120; /* (p-1)*(q-1) */
static const uint64_t e = 17;     /* public exponent */
static const uint64_t m = 42;     /* plaintext message */

/* ─── TODO 1: Greatest Common Divisor (Euclidean algorithm) ─── */
static uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/* ─── TODO 2: Extended Euclidean Algorithm ───
 *
 * Finds integers x, y such that: a*x + b*y = gcd(a, b)
 * Stores x in *px, y in *py.
 * Returns gcd(a, b).
 */
static int64_t ext_gcd(int64_t a, int64_t b, int64_t *px, int64_t *py) {
    if (b == 0) {
        *px = 1;
        *py = 0;
        return a;
    }
    int64_t x1, y1;
    int64_t g = ext_gcd(b, a % b, &x1, &y1);
    *px = y1;
    *py = x1 - (a / b) * y1;
    return g;
}

/* ─── TODO 3: Fast modular exponentiation ───
 *
 * Computes (base^exp) % mod using binary exponentiation.
 * Time complexity: O(log exp).
 */
static uint64_t fast_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

/* ─── TODO 4: Miller-Rabin primality test ───
 *
 * Determines whether n is a prime number using the Miller-Rabin
 * probabilistic primality test.
 *
 * Algorithm:
 *   1. Handle small cases: n < 2 → false; n < 4 → true; n even → false
 *   2. Write n-1 = d * 2^s  (factor out powers of 2)
 *   3. For each witness a in {2, 3, 5, 7, 11}:
 *      a. Compute x = a^d mod n
 *      b. If x == 1 or x == n-1, continue to next witness
 *      c. Repeat s-1 times: x = x^2 mod n
 *         If x == n-1, break (this witness passes)
 *      d. If x != n-1, return false (composite)
 *   4. Return true (likely prime)
 *
 * The witnesses {2, 3, 5, 7, 11} are sufficient for deterministic
 * results for all n < 2^64 (per OEIS A014233 / known bounds).
 */
static int is_prime(uint64_t num) {
    uint64_t witnesses[] = {2, 3, 5, 7, 11};
    int nw = 5;
    uint64_t d, s, a, x, r;

    if (num < 2) return 0;
    if (num == 2 || num == 3) return 1;
    if ((num & 1) == 0) return 0;

    /* Write num-1 = d * 2^s, with d odd */
    d = num - 1;
    s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        s++;
    }

    for (int i = 0; i < nw; i++) {
        a = witnesses[i];
        if (a >= num) continue;

        /* Compute x = a^d mod num */
        x = fast_pow(a, d, num);

        if (x == 1 || x == num - 1) continue;

        for (r = 0; r < s - 1; r++) {
            x = (x * x) % num;
            if (x == num - 1) break;
        }
        if (x != num - 1) return 0; /* composite */
    }
    return 1; /* likely prime */
}

/* ─── TODO 5: Key generation ───
 *
 * Generates RSA keypair from two primes.
 *   - Computes n = p*q, φ = (p-1)*(q-1)
 *   - Validates that e is coprime with φ (gcd(e, φ) == 1)
 *   - Computes d = e⁻¹ mod φ via extended Euclidean algorithm
 *
 * Outputs the key generation steps to stdout.
 * Returns 0 on success, 1 on failure (if e is invalid).
 */
static int gen_keys(uint64_t prime_p, uint64_t prime_q, uint64_t pub_e, uint64_t *out_n, uint64_t *out_phi,
                    int64_t *out_d) {
    uint64_t mod_n = prime_p * prime_q;
    uint64_t mod_phi = (prime_p - 1) * (prime_q - 1);

    *out_n = mod_n;
    *out_phi = mod_phi;

    printf("Step 2 — Key Generation:\n");
    printf("  n = p*q = %" PRIu64 "*%" PRIu64 " = %" PRIu64 "\n", prime_p, prime_q, mod_n);
    printf("  φ(n) = (p-1)*(q-1) = %" PRIu64 "*%" PRIu64 " = %" PRIu64 "\n", prime_p - 1, prime_q - 1, mod_phi);
    printf("  Select e = %" PRIu64 " (public exponent)\n", pub_e);

    /* Validate e: must be 1 < e < φ and gcd(e, φ) == 1 */
    if (pub_e <= 1 || pub_e >= mod_phi) {
        printf("  ERROR: e must satisfy 1 < e < φ(n)\n");
        return 1;
    }
    uint64_t g = gcd(pub_e, mod_phi);
    printf("  Verify gcd(e, φ) = gcd(%" PRIu64 ", %" PRIu64 ") = %" PRIu64 "\n", pub_e, mod_phi, g);
    if (g != 1) {
        printf("  ERROR: gcd(e, φ) != 1, cannot compute modular inverse\n");
        return 1;
    }
    printf("  [OK — e and φ are coprime]\n");

    /* Compute d via extended Euclidean */
    int64_t x, y;
    int64_t g2 = ext_gcd((int64_t)pub_e, (int64_t)mod_phi, &x, &y);
    printf("  ext_gcd(e=%" PRId64 ", φ=%" PRId64 ") → gcd=%" PRId64 ", x=%" PRId64 ", y=%" PRId64 "\n", (int64_t)pub_e,
           (int64_t)mod_phi, g2, x, y);
    printf("  Equation: e*x + φ*y = gcd  →  %" PRId64 "*%" PRId64 " + %" PRId64 "*%" PRId64 " = %" PRId64 "\n",
           (int64_t)pub_e, x, (int64_t)mod_phi, y, g2);

    /* d must be positive: d = x mod φ */
    int64_t d_val = x;
    while (d_val < 0) d_val += (int64_t)mod_phi;
    d_val = d_val % (int64_t)mod_phi;
    printf("  Private key d = x mod φ = %" PRId64 "\n", d_val);
    printf("  Verify: e*d mod φ = %" PRIu64 " %s\n", ((uint64_t)((int64_t)pub_e * d_val) % mod_phi),
           (((uint64_t)((int64_t)pub_e * d_val) % mod_phi) == 1) ? "[OK]" : "[FAIL]");

    *out_d = d_val;
    return 0;
}

/* ─── TODO 6: Encryption: c = m^e mod n ─── */
static uint64_t encrypt(uint64_t msg, uint64_t pub_exp, uint64_t modulus) { return fast_pow(msg, pub_exp, modulus); }

/* ─── TODO 7: Decryption: m = c^d mod n ─── */
static uint64_t decrypt(uint64_t cipher, uint64_t priv_exp, uint64_t modulus) {
    return fast_pow(cipher, priv_exp, modulus);
}

int main(void) {
    /* ─── Step 1: Print RSA parameters and verify primality ─── */
    printf("=== RSA Public-Key Crypto Demo (uint64 toy version) ===\n\n");
    printf("Step 1 — RSA Parameter Setup & Primality Verification:\n");
    printf("  Prime p = %" PRIu64 " — Miller-Rabin: ", p);
    printf(is_prime(p) ? "prime ✓" : "composite ✗");
    printf("\n");
    printf("  Prime q = %" PRIu64 " — Miller-Rabin: ", q);
    printf(is_prime(q) ? "prime ✓" : "composite ✗");
    printf("\n");
    printf("  Modulus n = p*q = %" PRIu64 "\n", n);
    printf("  Euler φ(n) = (p-1)*(q-1) = %" PRIu64 "\n", phi);
    printf("  Public exponent e = %" PRIu64 "\n", e);
    printf("  Plaintext message m = %" PRIu64 "\n\n", m);

    /* ─── Step 2: Key generation with e validation ─── */
    uint64_t gen_n, gen_phi;
    int64_t d;
    int ret = gen_keys(p, q, e, &gen_n, &gen_phi, &d);
    if (ret != 0) {
        printf("\nKey generation failed!\n");
        return 1;
    }
    printf("\n");

    /* ─── Step 3: Encryption ─── */
    printf("Step 3 — Encryption (c = m^e mod n):\n");
    uint64_t c = encrypt(m, e, n);
    printf("  c = %" PRIu64 "^%" PRIu64 " mod %" PRIu64 "\n", m, e, n);
    printf("    = fast_pow(%" PRIu64 ", %" PRIu64 ", %" PRIu64 ")\n", m, e, n);
    printf("    = %" PRIu64 "\n\n", c);

    /* ─── Step 4: Decryption ─── */
    printf("Step 4 — Decryption (m' = c^d mod n):\n");
    uint64_t mp = decrypt(c, (uint64_t)d, n);
    printf("  m' = %" PRIu64 "^%" PRId64 " mod %" PRIu64 "\n", c, d, n);
    printf("     = fast_pow(%" PRIu64 ", %" PRId64 ", %" PRIu64 ")\n", c, d, n);
    printf("     = %" PRIu64 "\n\n", mp);

    /* ─── Step 5: Verification ─── */
    printf("Step 5 — Verification:\n");
    printf("  Original message  m  = %" PRIu64 "\n", m);
    printf("  Decrypted message m' = %" PRIu64 "\n", mp);
    if (mp == m) {
        printf("  m' == m  →  RSA works! ✓\n");
    } else {
        printf("  m' != m  →  ERROR! ✗\n");
    }

    return 0;
}
