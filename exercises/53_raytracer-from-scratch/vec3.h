/* vec3.h — 3D Vector type for raytracer
 *
 * Provides Vec3 structure and basic vector operations
 * for the minimal raytracer.
 */
#ifndef VEC3_H
#define VEC3_H

#include <math.h>

typedef struct {
    double x, y, z;
} Vec3;

/* Construct a Vec3 from three components */
Vec3 v3(double x, double y, double z);

/* Vector arithmetic */
Vec3 v3_add(Vec3 a, Vec3 b);
Vec3 v3_sub(Vec3 a, Vec3 b);
Vec3 v3_mul(Vec3 a, double s);
Vec3 v3_div(Vec3 a, double s);

/* Dot product and length */
double v3_dot(Vec3 a, Vec3 b);
double v3_len(Vec3 v);

/* Normalize a vector (returns unit vector) */
Vec3 v3_norm(Vec3 v);

#endif /* VEC3_H */
