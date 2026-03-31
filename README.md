# Raytracer

A software raytracer written in C, built chapter by chapter following *The Ray Tracer Challenge* by Jamis Buck. Covers Chapters 1–6: tuples, matrices, transforms, ray-sphere intersections, and Phong shading.

No external libraries. Pure C99. Outputs a PPM image you can open in any image viewer.

![Rendered sphere](output/sphere.png)

---

## What it renders

A Phong-shaded sphere with a point light, written from scratch:

- **Ambient** — base fill so nothing is pure black
- **Diffuse** — surface facing the light is bright, edges fall off
- **Specular** — glossy highlight based on eye and reflection angle

---

## Project structure

```
raytracer/
  src/
    tuple.h       — points, vectors, colors, dot/cross/reflect
    matrix.h      — 4x4 matrix multiply, inverse, transforms
    canvas.h      — pixel buffer + PPM file writer
    ray.h         — ray casting, sphere intersection, normals
    light.h       — Phong shading model
    main.c        — render loop
  tests/
    test_all.c    — 37 tests, no external framework
  output/         — rendered images land here
  Makefile
  LICENSE
  README.md
```

---

## Installation

### Windows (VS Code)

**1. Install GCC**

Download the latest **winlibs** build (UCRT runtime, 64-bit, zip):
👉 https://winlibs.com/

Extract to `C:\gcc\` then add GCC to your PATH:

```
Windows Search → "Edit the system environment variables"
→ Environment Variables → Path → Edit → New
→ Add: C:\gcc\mingw64\bin
```

Open a new terminal and verify:

```cmd
gcc --version
```

You should see something like `gcc 14.x.x`.

**2. Clone or download the project**

```cmd
git clone https://github.com/AlborzNazari/raytracer.git
cd raytracer
```

Or download the ZIP from GitHub and extract it.

**3. Create the output folder**

```cmd
mkdir output
```

**4. Install a PPM viewer in VS Code**

`Ctrl+Shift+X` → search `PBM/PPM/PGM Viewer` → Install

This lets you view the rendered `.ppm` image directly inside VS Code without installing anything else.

---

### Linux (Ubuntu / Debian)

```bash
sudo apt install gcc make imagemagick -y
git clone https://github.com/AlborzNazari/raytracer.git
cd raytracer
mkdir -p output
```

---

### macOS

```bash
xcode-select --install   # installs clang/gcc if not present
git clone https://github.com/AlborzNazari/raytracer.git
cd raytracer
mkdir -p output
```

---

## Usage

### Run the tests first

Always run tests before rendering. All 37 should pass.

**Windows:**
```cmd
gcc -Wall -O2 -Isrc -o test_runner.exe tests/test_all.c -lm && test_runner.exe
```

**Linux / macOS:**
```bash
make test
```

Expected output:
```
PASSED: 37
All tests green.
```

---

### Render the image

**Windows:**
```cmd
gcc -Wall -O2 -Isrc -o raytracer.exe src/main.c -lm && raytracer.exe
```

**Linux / macOS:**
```bash
make run
```

Expected output:
```
Rendering 512x512...
Saved: output/sphere.ppm
```

---

### View the result

| Platform | How |
|----------|-----|
| VS Code (any OS) | Click `output/sphere.ppm` — PPM viewer extension renders it inline |
| Windows | Open with GIMP (https://gimp.org) |
| macOS | Double-click — Preview opens PPM natively |
| Linux | `eog output/sphere.ppm` or `feh output/sphere.ppm` |

**Convert to PNG (Linux/macOS with ImageMagick):**
```bash
convert output/sphere.ppm output/sphere.png
```

---

## Makefile targets (Linux / macOS)

| Command | What it does |
|---------|--------------|
| `make` | Compile the renderer |
| `make run` | Compile and render |
| `make test` | Compile and run all 37 tests |
| `make clean` | Remove binaries and output images |

---

## How it works — chapter by chapter

| Chapter | File | What it implements |
|---------|------|--------------------|
| 1 | `tuple.h` | Points, vectors, colors as `{x,y,z,w}`. All vector math. |
| 2 | `canvas.h` | Pixel buffer. PPM file writer. |
| 3 | `matrix.h` | 4×4 matrix multiply, transpose, inverse via cofactor expansion. |
| 4 | `matrix.h` | Translation, scaling, rotation (X/Y/Z), shearing factories. |
| 5 | `ray.h` | Ray definition, `position(t)`, sphere intersection (quadratic formula), hit detection. |
| 6 | `light.h` | Phong model: ambient + diffuse + specular. |

The render loop in `main.c` fires one ray per pixel from a fixed eye point through a virtual wall. For each hit, it computes the surface normal, then calls `lighting()` to get the pixel color.

---

## Customize the scene

Open `src/main.c` and change:

```c
/* Sphere color */
s.color = color(1.0f, 0.2f, 0.2f);   /* R, G, B — try (0.2, 0.6, 1.0) for blue */

/* Sphere shape — chain transforms */
Matrix scale   = scaling(1.0f, 1.3f, 1.0f);   /* stretch on Y */
Matrix rotate  = rotation_z(0.4f);             /* tilt */

/* Light position */
PointLight light = point_light(
    point(-10.0f, 10.0f, -10.0f),  /* try moving this around */
    color(1.0f, 1.0f, 1.0f)        /* white light */
);

/* Canvas size */
const int CANVAS_SIZE = 512;   /* increase to 1024 for higher resolution */
```

---

## What comes next (Chapters 7–12)

| Chapter | Feature |
|---------|---------|
| 7 | World & Camera — multiple objects, proper view transform |
| 8 | Shadows — cast a shadow ray back to the light |
| 9 | Planes — a floor shape |
| 10 | Patterns — stripes and gradients on surfaces |
| 11 | Reflections — recursive ray bouncing |
| 12 | Cubes — slab intersection method |

---

## License

MIT — see [LICENSE](LICENSE).

Based on concepts from *The Ray Tracer Challenge* by Jamis Buck.
Implementation written from scratch in C by Alborz Nazari.
