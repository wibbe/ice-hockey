#include "wasm4.h"
#include "assets.h"
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define SCALE   64

#define TOP                     16
#define SCREEN_CENTER           (SCREEN_SIZE / 2)
#define HEIGHT                  (SCREEN_SIZE - TOP)
#define RINK_CENTER             ((vec2_t){160, 88})
#define RINK_HEIGHT             (HEIGHT)
#define RINK_WIDTH              (240)

const uint8_t smiley[] = {
    0b11000011,
    0b10000001,
    0b00100100,
    0b00100100,
    0b00000000,
    0b00100100,
    0b10011001,
    0b11000011,
};


typedef struct vec2_t {
    float x;
    float y;
} vec2_t;

typedef struct line_t {
    uint16_t start;
    uint16_t end;
} line_t;

typedef struct collider_t {
    vec2_t *points;
    line_t *lines;
    int lines_count;
} collider_t;

typedef struct entity_t {
    vec2_t pos;
    vec2_t vel;
    float size;
} entity_t;

typedef struct player_t {
    entity_t ent;
} player_t;

typedef struct puck_t {
    entity_t ent;
} puck_t;

typedef struct team_t {
    int score;
    player_t players[5];
} team_t;

typedef struct game_t {
    team_t teams[2];
    puck_t puck;
    int camera;
} game_t;



static vec2_t rink_collider_points[] = {
    // Top
    (vec2_t) {34, 15},
    (vec2_t) {286, 15},

    // Right Side
    (vec2_t) {319, 49},
    (vec2_t) {319, 125},

    // Bottom
    (vec2_t) {285, 159},
    (vec2_t) {24, 159},

    // Left Side
    (vec2_t) {0, 125},
    (vec2_t) {0, 49},
};

static line_t rink_collider_lines[] = {
    // Top
    (line_t) {0, 1},
    // Right Side
    (line_t) {1, 2},
    (line_t) {2, 3},
    (line_t) {3, 4},
    // Bottom
    (line_t) {4, 5},
    // Left Side
    (line_t) {5, 6},
    (line_t) {6, 7},
    (line_t) {7, 0},
};

static collider_t rink_collider = {
    .points = rink_collider_points,
    .lines = rink_collider_lines,
    .lines_count = sizeof(rink_collider_lines) / sizeof(line_t),
};



static game_t game = {0};


int screen(float v) {
    return (int)round(v);
}

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


static bool collide_entity(entity_t *ent, collider_t *collider) {
    bool collision = false;

    for (int i = 0; i < collider->lines_count; ++i) {
        line_t line = collider->lines[i];
        vec2_t start = collider->points[line.start];
        vec2_t end = collider->points[line.end];

        float line_len = vlength(vsub(end, start));
        vec2_t line_dir = vscale(vsub(end, start), 1.0f / line_len);
        vec2_t line_norm = vperp(line_dir);
        vec2_t to_ent = vsub(ent->pos, start);

        vec2_t point;

        // Project entity position on to line
        float on_line = vdot(line_dir, to_ent);
        if (on_line < 0.0f) {
            point = start;
        } else if (on_line > line_len) {
            point = end;
        } else {
            point = vadd(start, vscale(line_dir, on_line));
        }

        float distance = vlength(vsub(point, ent->pos));
        if (distance < ent->size) {
            // We have a collision
            float overlap = ent->size - distance;

            ent->pos = vadd(ent->pos, vscale(line_norm, overlap));
            ent->vel = vscale(vreflect(ent->vel, line_norm), 1.0f);
            collision = true;
        }
    }

    return collision;
}

static void update_entity(entity_t* ent) {
    ent->pos = vadd(ent->pos, ent->vel);

    float speed = vlength(ent->vel);
    vec2_t dir = vscale(ent->vel, 1.0f / speed);

    vec2_t friction = vscale(dir, speed * -0.005f);
    //ent->vel = vadd(ent->vel, friction);

    if (vdot(ent->vel, dir) < 0.0f) {
        ent->vel = vzero();
    }

    collide_entity(ent, &rink_collider);
}

static void update_game(void) {
    update_entity(&game.puck.ent);
}


static void update_camera(void) {
    int camera_diff = screen(game.puck.ent.pos.x) - game.camera;
    if (camera_diff > 110) {
        game.camera += camera_diff - 110;
    }

    if (camera_diff < 50) {
        game.camera -= 50 - camera_diff;
    }

    if (game.camera < 0) {
        game.camera = 0;
    }
    if (game.camera > SCREEN_SIZE) {
        game.camera = SCREEN_SIZE;
    }
}


static void draw_puck(void) {
    *DRAW_COLORS = 4;
    blit(smiley, screen(game.puck.ent.pos.x) - 4 - game.camera, screen(game.puck.ent.pos.y) - 4, 8, 8, BLIT_1BPP);
}

static void draw(void) {
    *DRAW_COLORS = 0x4321;
    blit(rink, -game.camera, 0, rinkWidth, rinkHeight, rinkFlags);


    draw_puck();
}



static void new_game(void) {
    memset(&game, 0, sizeof(game_t));
    game.camera = SCREEN_SIZE / 2;

    // Puck
    game.puck.ent.pos = RINK_CENTER;

    game.puck.ent.vel.x = 2.0f;
    game.puck.ent.vel.y = 3.0f;
    game.puck.ent.size = 4.0f;
}

void start(void) {
    new_game();
}

void update() {
    update_game();
    update_camera();
    draw();
}
