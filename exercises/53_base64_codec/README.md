## Lesson 53 Base64 Encoder/Decoder Base64 编解码

### 代码

    /* 编码: 3字节 → 4个Base64字符 */
    int base64_encode(const unsigned char *src, int srclen,
                      char *dst, int dstcap)
    {
        int di = 0;
        for (int i = 0; i + 2 < srclen; i += 3) {
            unsigned int triplet = (src[i]<<16) | (src[i+1]<<8) | src[i+2];
            dst[di++] = b64_table[(triplet >> 18) & 0x3F];
            dst[di++] = b64_table[(triplet >> 12) & 0x3F];
            dst[di++] = b64_table[(triplet >>  6) & 0x3F];
            dst[di++] = b64_table[(triplet      ) & 0x3F];
        }
        /* ... 处理余数 1/2 字节 + padding '=' */
    }

### 知识点

- Base64 编码原理
  - 每 3 字节 (24 bits) 分为 4 组 6-bit 值
  - 6 bits → 64 种可能 → 用 A-Z/a-z/0-9/+/ 表示
  - 不足 3 字节时用 `=` 填充 (padding)
- 位操作技巧
  - 三字节拼接：`(b0 << 16) | (b1 << 8) | b2`
  - 6-bit 提取：`>> 18`, `>> 12`, `>> 6`, `& 0x3F`
- 查表法 (Lookup Table)
  - 编码：`b64_table[index]` — O(1) 正向映射
  - 解码：`b64_decode_table[char]` — 256 字节反向表，O(1) 查找

### 课堂讨论

- Base64 编码后数据膨胀了多少？为什么是 4/3 ≈ 133%？
- 为什么 Email 附件和 Data URI 都用 Base64？
- Base64url 变体 (`-` 和 `_` 替代 `+` 和 `/`) 用于什么场景？

### 课后练习

- 实现 Base64url 变体
- 从标准输入读取二进制文件，输出 Base64 (每 76 字符换行)
- 挑战：实现 Base32 编码 (5-bit 分组，32 字符集)

### 参考资料

- RFC 4648 - Base Encodings (Base16, Base32, Base64)
