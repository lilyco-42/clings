/* complex.h — Complex number type for qubit simulation
 *
 * A qubit state is a vector of two complex amplitudes: [α, β]
 * where α = amplitude for |0⟩, β = amplitude for |1⟩.
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
