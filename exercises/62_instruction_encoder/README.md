## Lesson 62 RISC-V Instruction Encoder RISC-V 指令编码器

### 代码

    #include <stdio.h>
    #include <stdint.h>

    /* RV32I 六种指令格式编码 */
    uint32_t encode_r(int opcode, int rd, int funct3, int rs1, int rs2, int funct7) {
        return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) |
               (funct3 << 12) | (rd << 7) | opcode;
    }

    uint32_t encode_i(int opcode, int rd, int funct3, int rs1, int imm) {
        return ((imm & 0xFFF) << 20) | (rs1 << 15) |
               (funct3 << 12) | (rd << 7) | opcode;
    }

### 知识点

- RV32I 六种指令格式
  - R-type: 寄存器 - 寄存器运算 (add, sub, mul)
  - I-type: 立即数运算和加载 (addi, lw, jalr)
  - S-type: 存储指令 (sw, sb)
  - B-type: 条件分支 (beq, bne, blt, bge)
  - U-type: 高位立即数 (lui, auipc)
  - J-type: 无条件跳转 (jal)
- 指令编码字段
  - opcode (6:0): 操作码，决定指令类型
  - rd (11:7): 目标寄存器
  - funct3 (14:12): 功能码，区分同类指令
  - rs1 (19:15): 源寄存器 1
  - rs2 (24:20): 源寄存器 2
  - funct7 (31:25): 扩展功能码
- 立即数编码
  - I-type: 12 位有符号立即数 imm[11:0]
  - S-type: imm[11:5] 和 imm[4:0] 分散存放
  - B-type: imm[12|10:5|4:1|11] 位域交错
  - J-type: imm[20|10:1|11|19:12] 位域交错

### 课堂讨论

- 为什么 RISC-V 的立即数位域是"交错"的？这样设计有什么好处？
- 为什么所有指令格式中 opcode、rd、rs1 的位置都是固定的？
- `add` 和 `sub` 的 opcode/funct3 完全相同，只靠 funct7 区分，为什么这样设计？

### 课后练习

- 手工计算 `addi x10, x0, 42` 的 32 位编码，并与 objdump 输出对比
- 扩展编码器支持 S-type 和 B-type 指令
- 用编码器生成 Lesson 61 的 return42.s 对应的二进制，与 GNU as 输出对比

### 参考资料

- RISC-V 指令编码规范 https://riscv.org/specifications/
- RISC-V 在线编码/解码器 https://luplab.gitlab.io/rvcodecjs/
