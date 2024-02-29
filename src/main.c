#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "wasm4.h"
#include "assets.h"

#define VEC2_IMPLEMENTATION
#include "vec2.h"

#define PHYSICS_IMPLEMENTATION
#include "physics.h"

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
    puck_t puck2;
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

static static_collider_t rink_collider = {
    .points = rink_collider_points,
    .lines = rink_collider_lines,
    .lines_count = sizeof(rink_collider_lines) / sizeof(line_t),
};



static game_t game = {0};


int screen(float v) {
    return (int)round(v);
}



static void update_entity(entity_t *ent) {
    simulate_entity(ent);

    if (static_collide_entity(ent, &rink_collider).collide) {
        tone(340, 5, 10, TONE_TRIANGLE);
    }
}


static void update_game(void) {
    update_entity(&game.puck.ent);
    update_entity(&game.puck2.ent);

    dynamic_collide_entity(&game.puck.ent, &game.puck2.ent);
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
    *DRAW_COLORS = 2;
    blit(smiley, screen(game.puck.ent.pos.x) - 4 - game.camera, screen(game.puck.ent.pos.y) - 4, 8, 8, BLIT_1BPP);

    *DRAW_COLORS = 3;
    blit(smiley, screen(game.puck2.ent.pos.x) - 4 - game.camera, screen(game.puck2.ent.pos.y) - 4, 8, 8, BLIT_1BPP);
}

static void draw(void) {
    *DRAW_COLORS = 0x4321;
    blit(rink, -game.camera, 0, rinkWidth, rinkHeight, rinkFlags);
    blit(rink, -game.camera + SCREEN_SIZE, 0, rinkWidth, rinkHeight, rinkFlags|BLIT_FLIP_X);


    draw_puck();
}



static void new_game(void) {
    memset(&game, 0, sizeof(game_t));
    game.camera = SCREEN_SIZE / 2;

    // Puck
    game.puck.ent.pos = vec(140, 87);
    game.puck2.ent.pos = vec(180, 89);

    game.puck.ent.vel.x = 1.5f;
    game.puck.ent.vel.y = 0.0f;
    game.puck.ent.size = 4.0f;

    game.puck2.ent.vel.x = -1.5f;
    game.puck2.ent.vel.y = 0.0f;
    game.puck2.ent.size = 4.0f;    
}

void start(void) {
    // Setup palette
    PALETTE[0] = 0xe9f4e1;
    PALETTE[1] = 0xd70f0f;
    PALETTE[2] = 0x0b3bf2;
    PALETTE[3] = 0x071821;



    new_game();
}

void update() {
    update_game();
    update_camera();
    draw();
}
