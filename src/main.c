#include <stdio.h>
#include <math.h>
#include "tuple.h"
#include "matrix.h"
#include "canvas.h"
#include "ray.h"
#include "light.h"

/* -------------------------------------------------------
 * RENDER
 *
 * This is the classic "put sphere on wall" setup from
 * Chapter 5-6. We cast one ray per pixel from a fixed
 * eye point through a virtual "wall" in front of it.
 * For each ray, we test intersection with the sphere.
 * If we hit it, we shade the pixel with Phong lighting.
 * ------------------------------------------------------- */
int main(void) {
    const int   CANVAS_SIZE  = 512;
    const float WALL_Z       = 10.0f;   /* wall sits at z=10  */
    const float WALL_SIZE    = 7.0f;    /* wall is 7x7 units  */
    const float PIXEL_SIZE   = WALL_SIZE / CANVAS_SIZE;
    const float HALF         = WALL_SIZE / 2.0f;

    Canvas canvas = canvas_create(CANVAS_SIZE, CANVAS_SIZE);

    /* --- Build the scene --- */

    /* Sphere: red, a bit stretched on y, rotated slightly */
    Sphere s = sphere_create();
    s.color     = color(1.0f, 0.2f, 0.2f);   /* red */
    s.diffuse   = 0.7f;
    s.specular  = 0.3f;
    s.shininess = 80.0f;

    /* Chain transforms: scale → rotate → done */
    Matrix scale  = scaling(1.0f, 1.3f, 1.0f);
    Matrix rotate  = rotation_z(0.4f);
    Matrix combined = matrix_mul(&rotate, &scale);
    sphere_set_transform(&s, &combined);

    /* Light: white, upper-left of scene */
    PointLight light = point_light(
        point(-10.0f, 10.0f, -10.0f),
        color(1.0f, 1.0f, 1.0f)
    );

    /* Eye/camera is at origin, looking in +z direction */
    Tuple ray_origin = point(0.0f, 0.0f, -5.0f);

    printf("Rendering %dx%d...\n", CANVAS_SIZE, CANVAS_SIZE);

    /* --- Cast a ray for every pixel --- */
    for (int py = 0; py < CANVAS_SIZE; py++) {
        /* World y coordinate: top of image = +HALF */
        float world_y = HALF - PIXEL_SIZE * py;

        for (int px = 0; px < CANVAS_SIZE; px++) {
            float world_x = -HALF + PIXEL_SIZE * px;

            /* Direction: from eye toward this pixel on the wall */
            Tuple wall_point   = point(world_x, world_y, WALL_Z);
            Tuple direction    = normalize(tuple_sub(wall_point, ray_origin));
            Ray   r            = ray(ray_origin, direction);

            /* Test intersection */
            Intersections xs = ray_intersect(r, &s);
            Intersection *h  = hit(&xs);

            if (h) {
                /* Compute shading inputs */
                Tuple pos     = ray_position(r, h->t);
                Tuple normalv = sphere_normal_at(&s, pos);
                Tuple eyev    = tuple_neg(r.direction);

                Tuple pixel_color = lighting(&s, &light, pos, eyev, normalv);
                canvas_write(&canvas, px, py, pixel_color);
            }
        }
    }

    /* Save output */
    const char *out = "output/sphere.ppm";
    if (canvas_write_ppm(&canvas, out))
        printf("Saved: %s\n", out);
    else
        fprintf(stderr, "Error writing %s\n", out);

    canvas_free(&canvas);
    return 0;
}
