#ifndef TUPLE_H
#define TUPLE_H

#include <math.h>

#define EPSILON 1e-5f

/* -------------------------------------------------------
 * TUPLE
 * The foundation of everything. A point has w=1, a
 * vector has w=0. This single type handles both.
 * ------------------------------------------------------- */
typedef struct {
    float x, y, z, w;
} Tuple;

/* Constructors */
static inline Tuple point(float x, float y, float z)  { return (Tuple){x, y, z, 1.0f}; }
static inline Tuple vector(float x, float y, float z) { return (Tuple){x, y, z, 0.0f}; }
static inline Tuple color(float r, float g, float b)  { return (Tuple){r, g, b, 0.0f}; }

/* Equality (floating point safe) */
static inline int tuple_eq(Tuple a, Tuple b) {
    return fabsf(a.x - b.x) < EPSILON &&
           fabsf(a.y - b.y) < EPSILON &&
           fabsf(a.z - b.z) < EPSILON &&
           fabsf(a.w - b.w) < EPSILON;
}

/* Arithmetic */
static inline Tuple tuple_add(Tuple a, Tuple b) { return (Tuple){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
static inline Tuple tuple_sub(Tuple a, Tuple b) { return (Tuple){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
static inline Tuple tuple_neg(Tuple a)           { return (Tuple){-a.x, -a.y, -a.z, -a.w}; }
static inline Tuple tuple_scale(Tuple a, float s){ return (Tuple){a.x*s, a.y*s, a.z*s, a.w*s}; }
static inline Tuple tuple_div(Tuple a, float s)  { return (Tuple){a.x/s, a.y/s, a.z/s, a.w/s}; }

/* Vector math */
static inline float magnitude(Tuple v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

static inline Tuple normalize(Tuple v) {
    float m = magnitude(v);
    return tuple_div(v, m);
}

static inline float dot(Tuple a, Tuple b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

/* Cross product — only makes sense for 3D vectors (w=0) */
static inline Tuple cross(Tuple a, Tuple b) {
    return vector(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

/* Color multiply (Hadamard product) */
static inline Tuple color_mul(Tuple a, Tuple b) {
    return color(a.x*b.x, a.y*b.y, a.z*b.z);
}

/* Reflect a vector around a normal */
static inline Tuple reflect(Tuple v, Tuple n) {
    return tuple_sub(v, tuple_scale(n, 2.0f * dot(v, n)));
}

#endif /* TUPLE_H */
