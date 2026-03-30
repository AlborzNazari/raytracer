#ifndef LIGHT_H
#define LIGHT_H

#include "tuple.h"
#include "ray.h"

/* -------------------------------------------------------
 * POINT LIGHT  (Chapter 6)
 * ------------------------------------------------------- */
typedef struct {
    Tuple position;
    Tuple intensity;   /* color/brightness */
} PointLight;

static inline PointLight point_light(Tuple position, Tuple intensity) {
    return (PointLight){ position, intensity };
}

/* -------------------------------------------------------
 * PHONG LIGHTING  (Chapter 6)
 *
 * Combines three components:
 *   Ambient  – base fill, independent of light direction
 *   Diffuse  – depends on angle between L and N
 *   Specular – shiny highlight, depends on angle between R and Eye
 * ------------------------------------------------------- */
static inline Tuple lighting(
    const Sphere    *sphere,
    const PointLight *light,
    Tuple            point_on_surface,
    Tuple            eyev,
    Tuple            normalv
) {
    /* Blend surface color with light intensity */
    Tuple effective_color = color_mul(sphere->color, light->intensity);

    /* Vector toward the light */
    Tuple lightv = normalize(tuple_sub(light->position, point_on_surface));

    /* Ambient contribution — always present */
    Tuple ambient = tuple_scale(effective_color, sphere->ambient);

    /* cos(angle between light and normal)
     * negative → light is behind the surface */
    float light_dot_normal = dot(lightv, normalv);

    Tuple diffuse  = color(0,0,0);
    Tuple specular = color(0,0,0);

    if (light_dot_normal >= 0.0f) {
        diffuse = tuple_scale(effective_color, sphere->diffuse * light_dot_normal);

        /* Reflection vector: R = I - 2(I·N)N */
        Tuple reflectv   = reflect(tuple_neg(lightv), normalv);
        float reflect_dot_eye = dot(reflectv, eyev);

        if (reflect_dot_eye > 0.0f) {
            float factor = powf(reflect_dot_eye, sphere->shininess);
            specular = tuple_scale(light->intensity, sphere->specular * factor);
        }
    }

    return tuple_add(tuple_add(ambient, diffuse), specular);
}

#endif /* LIGHT_H */
