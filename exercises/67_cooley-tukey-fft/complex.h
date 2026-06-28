/* complex.h — Complex number type for FFT
 *
 * Complex numbers are represented as:
 *   real + i * imag
 *
 * Key operations needed:
 *   - complex_add:  add two complex numbers
 *   - complex_sub:  subtract two complex numbers
 *   - complex_mul:  multiply two complex numbers
 *   - complex_print: print a complex number
 */
#ifndef COMPLEX_H
#define COMPLEX_H

#include <stdio.h>

typedef struct {
    double real;
    double imag;
} Complex;

Complex complex_add(Complex a, Complex b);
Complex complex_sub(Complex a, Complex b);
Complex complex_mul(Complex a, Complex b);
void complex_print(Complex c);

#endif /* COMPLEX_H */
