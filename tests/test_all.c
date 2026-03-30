/*
 * tests/test_all.c
 *
 * Minimal test runner — no external framework needed.
 * Add a new TEST() macro block for each feature.
 * Run with: make test
 */

#include <stdio.h>
#include <math.h>
#include "../src/tuple.h"
#include "../src/matrix.h"
#include "../src/canvas.h"
#include "../src/ray.h"
#include "../src/light.h"

/* ---- Test harness ---- */
static int _pass = 0, _fail = 0;
#define ASSERT(cond, msg) \
    do { \
        if (cond) { _pass++; } \
        else { fprintf(stderr, "  FAIL  %s  (line %d)\n", msg, __LINE__); _fail++; } \
    } while(0)

#define ASSERTF(a, b, msg) ASSERT(fabsf((a)-(b)) < EPSILON, msg)

/* ============================================================
 * Chapter 1: Tuples, Vectors, Points
 * ============================================================ */
void test_tuples(void) {
    printf("-- Ch1: Tuples\n");

    Tuple p = point(4, -4, 3);
    ASSERT(p.w == 1.0f, "point w=1");

    Tuple v = vector(4, -4, 3);
    ASSERT(v.w == 0.0f, "vector w=0");

    /* Addition: point + vector = point */
    Tuple sum = tuple_add(point(3,2,1), vector(5,6,7));
    ASSERT(tuple_eq(sum, point(8,8,8)), "point + vector");

    /* Subtraction: point - point = vector */
    Tuple diff = tuple_sub(point(3,2,1), point(5,6,7));
    ASSERT(tuple_eq(diff, vector(-2,-4,-6)), "point - point = vector");

    /* Magnitude */
    ASSERTF(magnitude(vector(1,2,3)), sqrtf(14.0f), "magnitude(1,2,3)");

    /* Normalize */
    Tuple n = normalize(vector(4,0,0));
    ASSERT(tuple_eq(n, vector(1,0,0)), "normalize");

    /* Dot */
    ASSERTF(dot(vector(1,2,3), vector(2,3,4)), 20.0f, "dot product");

    /* Cross */
    Tuple cr = cross(vector(1,2,3), vector(2,3,4));
    ASSERT(tuple_eq(cr, vector(-1,2,-1)), "cross product");

    /* Reflect */
    Tuple r = reflect(vector(1,-1,0), vector(0,1,0));
    ASSERT(tuple_eq(r, vector(1,1,0)), "reflect 45deg");
}

/* ============================================================
 * Chapter 2: Colors (stored as Tuples)
 * ============================================================ */
void test_colors(void) {
    printf("-- Ch2: Colors\n");

    Tuple c1 = color(0.9f, 0.6f, 0.75f);
    Tuple c2 = color(0.7f, 0.1f, 0.25f);

    Tuple added = tuple_add(c1, c2);
    ASSERTF(added.x, 1.6f, "color add r");

    Tuple mulc = color_mul(color(1,0.2f,0.4f), color(0.9f,1,0.1f));
    ASSERTF(mulc.x, 0.9f, "hadamard r");
    ASSERTF(mulc.z, 0.04f, "hadamard b");
}

/* ============================================================
 * Chapter 3: Matrices
 * ============================================================ */
void test_matrices(void) {
    printf("-- Ch3: Matrices\n");

    Matrix id = matrix_identity();
    Tuple  t  = point(1, 2, 3);
    Tuple  r  = matrix_mul_tuple(&id, t);
    ASSERT(tuple_eq(r, t), "identity * tuple = tuple");

    /* Transpose */
    Matrix a = {{{ 0,9,3,0 },{ 9,8,0,8 },{ 1,8,5,3 },{ 0,0,5,8 }}};
    Matrix at = matrix_transpose(&a);
    ASSERTF(at.m[0][1], 9.0f, "transpose [0][1]");
    ASSERTF(at.m[1][0], 9.0f, "transpose [1][0]");

    /* Inverse: A * inv(A) ≈ I */
    Matrix b = {{{ 8,-5, 9, 2 },{ 7, 5, 6, 1 },{ -6, 8, 2, 6 },{ -3, 0, 9, 5 }}};
    Matrix inv;
    ASSERT(matrix_inverse(&b, &inv), "inverse exists");
    Matrix should_be_id = matrix_mul(&b, &inv);
    Matrix ident = matrix_identity();
    ASSERT(matrix_eq(&should_be_id, &ident), "A * inv(A) = I");
}

/* ============================================================
 * Chapter 4: Transforms
 * ============================================================ */
void test_transforms(void) {
    printf("-- Ch4: Transforms\n");

    /* Translation moves a point */
    Matrix tr = translation(5, -3, 2);
    Tuple  moved = matrix_mul_tuple(&tr, point(-3, 4, 5));
    ASSERT(tuple_eq(moved, point(2, 1, 7)), "translation point");

    /* Translation does NOT move vectors */
    Matrix tr2 = translation(5, -3, 2);
    Tuple  v   = vector(-3, 4, 5);
    Tuple  unchanged = matrix_mul_tuple(&tr2, v);
    ASSERT(tuple_eq(unchanged, v), "translation vector unchanged");

    /* Scaling */
    Matrix sc = scaling(2, 3, 4);
    Tuple  sp = matrix_mul_tuple(&sc, point(-4, 6, 8));
    ASSERT(tuple_eq(sp, point(-8, 18, 32)), "scaling point");

    /* Rotation on X */
    Matrix rx = rotation_x(3.14159f / 2.0f);    /* 90 degrees */
    Tuple  rp = matrix_mul_tuple(&rx, point(0, 1, 0));
    ASSERT(fabsf(rp.y) < 0.001f && fabsf(rp.z - 1.0f) < 0.001f, "rotate_x 90deg");
}

/* ============================================================
 * Chapter 5: Rays & Intersections
 * ============================================================ */
void test_rays(void) {
    printf("-- Ch5: Rays & Intersections\n");

    /* position(r, t) */
    Ray r = ray(point(2,3,4), vector(1,0,0));
    ASSERT(tuple_eq(ray_position(r, 0.0f),  point(2,3,4)),  "ray pos t=0");
    ASSERT(tuple_eq(ray_position(r, 1.0f),  point(3,3,4)),  "ray pos t=1");
    ASSERT(tuple_eq(ray_position(r, -1.0f), point(1,3,4)),  "ray pos t=-1");

    /* Ray through middle of unit sphere */
    Sphere s = sphere_create();
    Ray r2 = ray(point(0,0,-5), vector(0,0,1));
    Intersections xs = ray_intersect(r2, &s);
    ASSERT(xs.count == 2,                  "sphere: two hits");
    ASSERTF(xs.xs[0].t, 4.0f,             "sphere: t1=4");
    ASSERTF(xs.xs[1].t, 6.0f,             "sphere: t2=6");

    /* Tangent */
    Ray r3 = ray(point(0,1,-5), vector(0,0,1));
    Intersections xs3 = ray_intersect(r3, &s);
    ASSERT(xs3.count == 2,                 "tangent: two equal hits");
    ASSERTF(xs3.xs[0].t, 5.0f,            "tangent: t=5");

    /* Miss */
    Ray r4 = ray(point(0,2,-5), vector(0,0,1));
    Intersections xs4 = ray_intersect(r4, &s);
    ASSERT(xs4.count == 0,                 "miss: count=0");

    /* hit() ignores negative t */
    Intersections xs5 = { .count = 0 };
    xs_add(&xs5, -1.0f, &s);
    xs_add(&xs5, 1.0f,  &s);
    Intersection *h = hit(&xs5);
    ASSERTF(h->t, 1.0f, "hit ignores negative");

    /* Transform ray */
    Ray r5 = ray(point(1,2,3), vector(0,1,0));
    Matrix tr = translation(3,4,5);
    Ray tr_r = ray_transform(r5, &tr);
    ASSERT(tuple_eq(tr_r.origin, point(4,6,8)),    "transform ray origin");
    ASSERT(tuple_eq(tr_r.direction, vector(0,1,0)), "transform ray dir");

    /* Normal at point on sphere */
    Tuple n = sphere_normal_at(&s, point(1,0,0));
    ASSERT(tuple_eq(n, vector(1,0,0)), "normal at (1,0,0)");
}

/* ============================================================
 * Chapter 6: Lighting
 * ============================================================ */
void test_lighting(void) {
    printf("-- Ch6: Lighting\n");

    Sphere s = sphere_create();

    /* Eye directly in front, light directly in front → full ambient+diffuse */
    PointLight light = point_light(point(0,0,-10), color(1,1,1));
    Tuple eyev   = vector(0,0,-1);
    Tuple normalv = vector(0,0,-1);
    Tuple result = lighting(&s, &light, point(0,0,0), eyev, normalv);
    ASSERTF(result.x, 1.9f, "full front lighting r");

    /* Eye offset 45deg — specular drops out */
    Tuple eyev2  = vector(0, sqrtf(2)/2, -sqrtf(2)/2);
    Tuple result2 = lighting(&s, &light, point(0,0,0), eyev2, normalv);
    ASSERTF(result2.x, 1.0f, "eye offset: no specular");

    /* Light at 45deg — diffuse reduced, no specular */
    PointLight light2 = point_light(point(0,10,-10), color(1,1,1));
    Tuple result3 = lighting(&s, &light2, point(0,0,0), eyev, normalv);
    ASSERTF(result3.x, 0.7364f, "light offset");
}

/* ============================================================
 * MAIN
 * ============================================================ */
int main(void) {
    printf("============================\n");
    printf("  Ray Tracer Test Suite\n");
    printf("============================\n");

    test_tuples();
    test_colors();
    test_matrices();
    test_transforms();
    test_rays();
    test_lighting();

    printf("----------------------------\n");
    printf("  PASSED: %d\n", _pass);
    if (_fail > 0)
        printf("  FAILED: %d  ← fix these\n", _fail);
    else
        printf("  All tests green.\n");
    printf("============================\n");
    return _fail > 0 ? 1 : 0;
}
