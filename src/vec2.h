
#ifndef VEC2_H
#define VEC2_H

typedef struct vec2_t {
    float x;
    float y;
} vec2_t;


vec2_t vec(float x, float y);
vec2_t vzero(void);

vec2_t vadd(vec2_t a, vec2_t b);
vec2_t vsub(vec2_t a, vec2_t b);
vec2_t vmul(vec2_t a, vec2_t b);
vec2_t vscale(vec2_t a, float b);

float vdot(vec2_t a, vec2_t b);
float vlength(vec2_t v);
vec2_t vperp(vec2_t v);
vec2_t vreflect(vec2_t v, vec2_t normal);
vec2_t vnormalized(vec2_t v);

#endif


#ifdef VEC2_IMPLEMENTATION


vec2_t vzero(void) {
    return (vec2_t) {
        0.0f, 0.0f
    };
}

vec2_t vec(float x, float y) {
    return (vec2_t) {
        x, y
    };
}

vec2_t vadd(vec2_t a, vec2_t b) {
    return (vec2_t) {
        a.x + b.x,
        a.y + b.y
    };
}

vec2_t vsub(vec2_t a, vec2_t b) {
    return (vec2_t) {
        a.x - b.x,
        a.y - b.y
    };
}

vec2_t vmul(vec2_t a, vec2_t b) {
    return (vec2_t) {
        a.x * b.x,
        a.y * b.y
    };
}

vec2_t vscale(vec2_t a, float b) {
    return (vec2_t) {
        a.x * b,
        a.y * b
    };
}

float vdot(vec2_t a, vec2_t b) {
    return a.x * b.x + a.y * b.y;
}

float vlength(vec2_t v) {
    return (float)sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vperp(vec2_t v) {
    return (vec2_t) {
        -v.y, v.x
    };
}

vec2_t vreflect(vec2_t v, vec2_t normal) {
    return vsub(v, vscale(normal, 2.0f * vdot(v, normal)));
}

vec2_t vnormalized(vec2_t v) {
    float len = vlength(v);

    if (len == 0)
        return v;

    len = 1.0f / len;

    return (vec2_t) {
        v.x * len,
        v.y * len
    };
}

#undef VEC2_IMPLEMENTATION
#endif
