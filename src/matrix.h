#ifndef MATRIX_H
#define MATRIX_H

#include <string.h>
#include "tuple.h"

/* -------------------------------------------------------
 * MATRIX  (4x4, row-major)
 * Chapters 3 & 4: the engine behind every transform.
 * ------------------------------------------------------- */
typedef struct {
    float m[4][4];
} Matrix;

static inline Matrix matrix_identity(void) {
    Matrix r = {{{0}}};
    r.m[0][0] = r.m[1][1] = r.m[2][2] = r.m[3][3] = 1.0f;
    return r;
}

static inline int matrix_eq(const Matrix *a, const Matrix *b) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (fabsf(a->m[i][j] - b->m[i][j]) >= EPSILON) return 0;
    return 1;
}

static inline Matrix matrix_mul(const Matrix *a, const Matrix *b) {
    Matrix r = {{{0}}};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                r.m[i][j] += a->m[i][k] * b->m[k][j];
    return r;
}

/* Multiply matrix by tuple (column vector) */
static inline Tuple matrix_mul_tuple(const Matrix *m, Tuple t) {
    float v[4] = { t.x, t.y, t.z, t.w };
    float r[4] = {0};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r[i] += m->m[i][j] * v[j];
    return (Tuple){r[0], r[1], r[2], r[3]};
}

static inline Matrix matrix_transpose(const Matrix *a) {
    Matrix r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.m[i][j] = a->m[j][i];
    return r;
}

/* 2x2 determinant helper */
static inline float det2(float a, float b, float c, float d) {
    return a*d - b*c;
}

/* 3x3 determinant via cofactor expansion */
static float det3(float m[3][3]) {
    return m[0][0] * det2(m[1][1],m[1][2],m[2][1],m[2][2])
          -m[0][1] * det2(m[1][0],m[1][2],m[2][0],m[2][2])
          +m[0][2] * det2(m[1][0],m[1][1],m[2][0],m[2][1]);
}

/* 4x4 minor: remove row r, col c → 3x3 submatrix */
static float minor4(const Matrix *a, int row, int col) {
    float sub[3][3];
    int si = 0;
    for (int i = 0; i < 4; i++) {
        if (i == row) continue;
        int sj = 0;
        for (int j = 0; j < 4; j++) {
            if (j == col) continue;
            sub[si][sj++] = a->m[i][j];
        }
        si++;
    }
    return det3(sub);
}

static float cofactor4(const Matrix *a, int row, int col) {
    float min = minor4(a, row, col);
    return ((row + col) % 2 == 0) ? min : -min;
}

static float det4(const Matrix *a) {
    float d = 0;
    for (int j = 0; j < 4; j++)
        d += a->m[0][j] * cofactor4(a, 0, j);
    return d;
}

/* Returns 0 if not invertible */
static inline int matrix_inverse(const Matrix *a, Matrix *out) {
    float d = det4(a);
    if (fabsf(d) < EPSILON) return 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out->m[j][i] = cofactor4(a, i, j) / d;  /* note transposed */
    return 1;
}

/* -------------------------------------------------------
 * TRANSFORM FACTORIES  (Chapter 4)
 * ------------------------------------------------------- */
static inline Matrix translation(float tx, float ty, float tz) {
    Matrix m = matrix_identity();
    m.m[0][3] = tx; m.m[1][3] = ty; m.m[2][3] = tz;
    return m;
}

static inline Matrix scaling(float sx, float sy, float sz) {
    Matrix m = matrix_identity();
    m.m[0][0] = sx; m.m[1][1] = sy; m.m[2][2] = sz;
    return m;
}

static inline Matrix rotation_x(float r) {
    Matrix m = matrix_identity();
    m.m[1][1] =  cosf(r); m.m[1][2] = -sinf(r);
    m.m[2][1] =  sinf(r); m.m[2][2] =  cosf(r);
    return m;
}

static inline Matrix rotation_y(float r) {
    Matrix m = matrix_identity();
    m.m[0][0] =  cosf(r); m.m[0][2] =  sinf(r);
    m.m[2][0] = -sinf(r); m.m[2][2] =  cosf(r);
    return m;
}

static inline Matrix rotation_z(float r) {
    Matrix m = matrix_identity();
    m.m[0][0] =  cosf(r); m.m[0][1] = -sinf(r);
    m.m[1][0] =  sinf(r); m.m[1][1] =  cosf(r);
    return m;
}

static inline Matrix shearing(float xy, float xz, float yx, float yz, float zx, float zy) {
    Matrix m = matrix_identity();
    m.m[0][1]=xy; m.m[0][2]=xz;
    m.m[1][0]=yx; m.m[1][2]=yz;
    m.m[2][0]=zx; m.m[2][1]=zy;
    return m;
}

#endif /* MATRIX_H */
