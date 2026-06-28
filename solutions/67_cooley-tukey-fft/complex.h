/* complex.h — Complex number type for FFT
 *
 * A frequency-domain sample is a complex number:
 *   real + i * imag
 *
 * Complex numbers are represented as:
 *   real + i * imag
 *
 * Key operations needed:
 *   - complex_mult: multiply two complex numbers
 *   - complex_add:  add two complex numbers
 */
#ifndef COMPLEX_H
#define COMPLEX_H

typedef struct {
    double real;
    double imag;
} Complex;

#endif /* COMPLEX_H */
