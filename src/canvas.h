#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include "tuple.h"

/* -------------------------------------------------------
 * CANVAS  (Chapter 2)
 * A flat pixel buffer. write_ppm() dumps it to a file
 * you can open in any image viewer.
 * ------------------------------------------------------- */
typedef struct {
    int     width;
    int     height;
    Tuple  *pixels;   /* row-major: pixel[y * width + x] */
} Canvas;

static inline Canvas canvas_create(int w, int h) {
    Canvas c;
    c.width  = w;
    c.height = h;
    c.pixels = (Tuple *)calloc((size_t)(w * h), sizeof(Tuple));
    return c;
}

static inline void canvas_free(Canvas *c) {
    free(c->pixels);
    c->pixels = NULL;
}

static inline void canvas_write(Canvas *c, int x, int y, Tuple col) {
    if (x < 0 || x >= c->width || y < 0 || y >= c->height) return;
    c->pixels[y * c->width + x] = col;
}

static inline Tuple canvas_read(const Canvas *c, int x, int y) {
    return c->pixels[y * c->width + x];
}

/* Clamp float 0..1 → int 0..255 */
static inline int clamp_color(float v) {
    if (v <= 0.0f) return 0;
    if (v >= 1.0f) return 255;
    return (int)(v * 255.0f + 0.5f);
}

/* Write PPM file — open with any image viewer or convert with ImageMagick */
static inline int canvas_write_ppm(const Canvas *c, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    fprintf(f, "P3\n%d %d\n255\n", c->width, c->height);
    int col = 0;
    for (int i = 0; i < c->width * c->height; i++) {
        Tuple px = c->pixels[i];
        /* PPM lines should not exceed 70 chars; we keep it simple */
        fprintf(f, "%d %d %d",
            clamp_color(px.x),
            clamp_color(px.y),
            clamp_color(px.z));
        col++;
        if (col % (c->width) == 0) { fprintf(f, "\n"); col = 0; }
        else fprintf(f, "  ");
    }
    fprintf(f, "\n");
    fclose(f);
    return 1;
}

#endif /* CANVAS_H */
