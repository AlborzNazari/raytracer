#ifndef RAY_H
#define RAY_H

#include <math.h>
#include <stdlib.h>
#include "tuple.h"
#include "matrix.h"

/* -------------------------------------------------------
 * RAY  (Chapter 5)
 * origin: a Point  |  direction: a Vector
 * position(t) = origin + t * direction
 * ------------------------------------------------------- */
typedef struct {
    Tuple origin;
    Tuple direction;
} Ray;

static inline Ray ray(Tuple origin, Tuple direction) {
    return (Ray){ origin, direction };
}

static inline Tuple ray_position(Ray r, float t) {
    return tuple_add(r.origin, tuple_scale(r.direction, t));
}

/* Transform a ray by a matrix (for moving objects into local space) */
static inline Ray ray_transform(Ray r, const Matrix *m) {
    return (Ray){
        matrix_mul_tuple(m, r.origin),
        matrix_mul_tuple(m, r.direction)
    };
}

/* -------------------------------------------------------
 * SPHERE
 * Unit sphere at origin. Give it a transform to move/scale.
 * ------------------------------------------------------- */
typedef struct {
    int    id;
    Matrix transform;        /* world→local transform   */
    Matrix transform_inv;    /* pre-computed inverse    */
    Tuple  color;            /* surface color           */
    float  ambient;
    float  diffuse;
    float  specular;
    float  shininess;
} Sphere;

static int _sphere_next_id = 0;

static inline Sphere sphere_create(void) {
    Sphere s;
    s.id            = _sphere_next_id++;
    s.transform     = matrix_identity();
    s.transform_inv = matrix_identity();
    s.color         = color(1, 1, 1);
    s.ambient       = 0.1f;
    s.diffuse       = 0.9f;
    s.specular      = 0.9f;
    s.shininess     = 200.0f;
    return s;
}

static inline void sphere_set_transform(Sphere *s, const Matrix *m) {
    s->transform = *m;
    matrix_inverse(m, &s->transform_inv);
}

/* -------------------------------------------------------
 * INTERSECTION  (Chapter 5)
 * Tracks which t value AND which sphere was hit.
 * ------------------------------------------------------- */
#define MAX_INTERSECTIONS 16

typedef struct {
    float   t;
    Sphere *object;
} Intersection;

typedef struct {
    Intersection xs[MAX_INTERSECTIONS];
    int          count;
} Intersections;

static inline void xs_add(Intersections *xs, float t, Sphere *obj) {
    if (xs->count < MAX_INTERSECTIONS) {
        xs->xs[xs->count].t      = t;
        xs->xs[xs->count].object = obj;
        xs->count++;
    }
}

/* Intersect a ray with a sphere — quadratic formula */
static inline Intersections ray_intersect(Ray r, Sphere *s) {
    Intersections result = { .count = 0 };

    /* Transform ray into object (local) space */
    Ray local_ray = ray_transform(r, &s->transform_inv);

    /* Vector from sphere center to ray origin */
    Tuple sphere_to_ray = tuple_sub(local_ray.origin, point(0,0,0));

    float a = dot(local_ray.direction, local_ray.direction);
    float b = 2.0f * dot(local_ray.direction, sphere_to_ray);
    float c = dot(sphere_to_ray, sphere_to_ray) - 1.0f;

    float discriminant = b*b - 4.0f*a*c;
    if (discriminant < 0.0f) return result;   /* miss */

    float sq = sqrtf(discriminant);
    xs_add(&result, (-b - sq) / (2.0f * a), s);
    xs_add(&result, (-b + sq) / (2.0f * a), s);
    return result;
}

/* Hit = closest non-negative t */
static inline Intersection *hit(Intersections *xs) {
    Intersection *best = NULL;
    for (int i = 0; i < xs->count; i++) {
        if (xs->xs[i].t >= 0.0f) {
            if (!best || xs->xs[i].t < best->t)
                best = &xs->xs[i];
        }
    }
    return best;
}

/* -------------------------------------------------------
 * NORMAL at a point on a sphere surface
 * ------------------------------------------------------- */
static inline Tuple sphere_normal_at(const Sphere *s, Tuple world_point) {
    /* Convert world point to object space */
    Tuple obj_point  = matrix_mul_tuple(&s->transform_inv, world_point);
    Tuple obj_normal = tuple_sub(obj_point, point(0,0,0));

    /* Transform normal back — use transpose of inverse */
    Matrix inv_T = matrix_transpose(&s->transform_inv);
    Tuple  world_normal = matrix_mul_tuple(&inv_T, obj_normal);
    world_normal.w = 0;  /* force vector */
    return normalize(world_normal);
}

#endif /* RAY_H */
