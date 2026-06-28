/* 53_raytracer-from-scratch.c — solution
 *
 * A minimal raytracer that renders two spheres with Phong illumination
 * and outputs a PPM P3 (ASCII) image.
 *
 * Scene:
 *   - Red sphere:   center=(0, 0, -5), radius=1
 *   - Blue sphere:  center=(1, -0.5, -4), radius=0.8
 *   - Point light:  position=(0, 10, -3)
 *   - Camera:       origin=(0, 0, 0), looking along -Z
 *   - Image plane:  z = -1, with viewport width=2, height=1
 *   - Resolution:   64x32 pixels
 */

#include <math.h>
#include <stdio.h>

#include "vec3.h"

/* ─── Vector operations ─── */

Vec3 v3(double x, double y, double z) {
    Vec3 v = {x, y, z};
    return v;
}

Vec3 v3_add(Vec3 a, Vec3 b) { return v3(a.x + b.x, a.y + b.y, a.z + b.z); }
Vec3 v3_sub(Vec3 a, Vec3 b) { return v3(a.x - b.x, a.y - b.y, a.z - b.z); }
Vec3 v3_mul(Vec3 a, double s) { return v3(a.x * s, a.y * s, a.z * s); }
Vec3 v3_div(Vec3 a, double s) { return v3(a.x / s, a.y / s, a.z / s); }
double v3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
double v3_len(Vec3 v) { return sqrt(v3_dot(v, v)); }
Vec3 v3_norm(Vec3 v) {
    double l = v3_len(v);
    return l > 0.0 ? v3_div(v, l) : v3(0, 0, 0);
}

/* ─── Ray ─── */

typedef struct {
    Vec3 orig, dir;
} Ray;

static inline Vec3 ray_at(Ray r, double t) { return v3_add(r.orig, v3_mul(r.dir, t)); }

/* ─── Sphere ─── */

typedef struct {
    Vec3 center;
    double radius;
    Vec3 color;
} Sphere;

/* ─── Light ─── */

typedef struct {
    Vec3 pos;
    Vec3 color;
} Light;

/* ─── Scene definition ─── */

#define WIDTH 64
#define HEIGHT 32

/* Ray-sphere intersection.
 * Returns the smallest positive t, or -1.0 if no hit. */
static double hit_sphere(Ray r, Sphere s) {
    Vec3 oc = v3_sub(r.orig, s.center);
    double a = v3_dot(r.dir, r.dir);
    double b = 2.0 * v3_dot(oc, r.dir);
    double c = v3_dot(oc, oc) - s.radius * s.radius;
    double disc = b * b - 4.0 * a * c;
    if (disc < 0.0) return -1.0;
    double sqrt_disc = sqrt(disc);
    double t0 = (-b - sqrt_disc) / (2.0 * a);
    double t1 = (-b + sqrt_disc) / (2.0 * a);
    if (t0 > 0.0) return t0;
    if (t1 > 0.0) return t1;
    return -1.0;
}

/* Clamp a double to [0, 1] and convert to 0-255 integer. */
static int to_byte(double x) {
    if (x < 0.0) x = 0.0;
    if (x > 1.0) x = 1.0;
    return (int)(x * 255.0 + 0.5);
}

int main(void) {
    /* Scene objects */
    Sphere spheres[2] = {
        {v3(0, 0, -5), 1.0, v3(1, 0, 0)},    /* Red sphere */
        {v3(1, -0.5, -4), 0.8, v3(0, 0, 1)}, /* Blue sphere */
    };
    int n_spheres = 2;

    Light light = {v3(0, 10, -3), v3(1, 1, 1)};

    /* Camera: origin at (0,0,0), image plane at z=-1 */
    Vec3 cam = v3(0, 0, 0);
    double viewport_w = 2.0;
    double viewport_h = 1.0;
    double focal = 1.0;

    /* PPM P3 header */
    printf("P3\n%d %d\n255\n", WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            /* Compute ray direction through pixel center */
            double u = (x + 0.5) / WIDTH;  /* [0, 1] */
            double v = (y + 0.5) / HEIGHT; /* [0, 1] */
            double px = (u - 0.5) * viewport_w;
            double py = (0.5 - v) * viewport_h; /* flip Y: PPM top row first */
            double pz = -focal;

            Vec3 dir = v3_norm(v3(px, py, pz));
            Ray ray = {cam, dir};

            /* Find the closest intersection */
            double closest_t = -1.0;
            int hit_idx = -1;
            for (int i = 0; i < n_spheres; i++) {
                double t = hit_sphere(ray, spheres[i]);
                if (t > 0.0 && (closest_t < 0.0 || t < closest_t)) {
                    closest_t = t;
                    hit_idx = i;
                }
            }

            Vec3 pixel_color;
            if (hit_idx >= 0) {
                /* Hit point and surface normal */
                Vec3 P = ray_at(ray, closest_t);
                Vec3 N = v3_norm(v3_sub(P, spheres[hit_idx].center));
                Vec3 obj_color = spheres[hit_idx].color;

                /* Light direction */
                Vec3 L = v3_norm(v3_sub(light.pos, P));
                /* View direction */
                Vec3 V = v3_norm(v3_sub(cam, P));
                /* Reflection direction */
                double ndotl = v3_dot(N, L);
                Vec3 R = v3_sub(v3_mul(N, 2.0 * ndotl), L);

                /* Ambient */
                double ambient_k = 0.1;
                Vec3 ambient = v3_mul(obj_color, ambient_k);

                /* Diffuse (Lambertian) */
                Vec3 diffuse = v3(0, 0, 0);
                if (ndotl > 0.0) {
                    double diff_k = 0.7;
                    diffuse = v3_mul(obj_color, diff_k * ndotl);
                }

                /* Specular (Phong) */
                Vec3 specular = v3(0, 0, 0);
                double rdotv = v3_dot(R, V);
                if (rdotv > 0.0 && ndotl > 0.0) {
                    double spec_k = 0.5;
                    double spec = pow(rdotv, 32.0);
                    specular = v3_mul(light.color, spec_k * spec);
                }

                pixel_color = v3_add(v3_add(ambient, diffuse), specular);
            } else {
                /* Background: black */
                pixel_color = v3(0, 0, 0);
            }

            /* Output pixel */
            printf("%d %d %d\n", to_byte(pixel_color.x), to_byte(pixel_color.y), to_byte(pixel_color.z));
        }
    }

    return 0;
}
