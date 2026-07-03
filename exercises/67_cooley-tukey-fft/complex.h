/* complex.h — FFT 使用的复数类型
 *
 * 频域样本用一个复数表示：
 *   real + i * imag
 *
 * 复数的加、减、乘运算在 fft.c 中内联实现（本头文件只定义数据类型）。
 */
#ifndef COMPLEX_H
#define COMPLEX_H

typedef struct {
    double real;
    double imag;
} Complex;

#endif /* COMPLEX_H */
