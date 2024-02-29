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

enum {
    PLAYER_GOLIE = 0,
    PLAYER_DEFENDER1,
    PLAYER_DEFENDER2,
    PLAYER_ATTACKER1,
    PLAYER_ATTACKER2,
    PLAYER_COUNT
};

enum {
    TEAM_RED = 0,
    TEAM_BLUE,
};



typedef struct player_t {
    entity_t ent;
    vec2_t dir;
} player_t;

typedef struct puck_t {
    entity_t ent;
    player_t *owner;
} puck_t;

typedef struct team_t {
    int score;
    player_t players[PLAYER_COUNT];
    int active_player;
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

static static_collider_t rink_collider = {
    .points = rink_collider_points,
    .lines = rink_collider_lines,
    .lines_count = sizeof(rink_collider_lines) / sizeof(line_t),
};

static vec2_t player_lineup[] = {
    (vec2_t) {34, 86},
    (vec2_t) {110, 63},
    (vec2_t) {100, 112},
    (vec2_t) {154, 87},
    (vec2_t) {142, 108},
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

static void update_puck(void) {
    if (game.puck.owner == NULL) {
        update_entity(&game.puck.ent);

        // Check distance to all players, to see if they can take possession of the puck
        for (int i = 0; i < PLAYER_COUNT; ++i) {
            player_t *player = &game.teams[0].players[i];
            if (vlength(vsub(player->ent.pos, game.puck.ent.pos)) < 6.0f) {
                game.puck.owner = player;
                game.teams[0].active_player = i;
            }
        }
    } else {
        game.puck.ent.pos = vadd(game.puck.owner->ent.pos, vscale(game.puck.owner->dir, 8.0f));
    }
}

static void update_team(team_t *team, uint8_t input) {
    bool left = input & BUTTON_LEFT;    
    bool right = input & BUTTON_RIGHT;    
    bool up = input & BUTTON_UP;    
    bool down = input & BUTTON_DOWN;
    bool shoot = input & BUTTON_1;
    bool pass = input & BUTTON_2;

    if (left || right || up || down) {
        vec2_t vel = vzero();
        if (left)
            vel.x -= 1;
        if (right)
            vel.x = 1;
        if (up)
            vel.y = -1;
        if (down)
            vel.y = 1;

        vel = vnormalized(vel);
        team->players[team->active_player].dir = vel;
        team->players[team->active_player].ent.vel = vel;

    } else {
        vec2_t vel = team->players[team->active_player].ent.vel;
        team->players[team->active_player].ent.vel = vscale(vel, 0.9f);
    }

    for (int i = 0; i < PLAYER_COUNT; ++i) {
        player_t *player = &team->players[i];

        if (i != team->active_player) {
            player->ent.vel = vscale(player->ent.vel, 0.9f);
        }

        update_entity(&player->ent);

        if (game.puck.owner == player) {
            if (shoot) {
                game.puck.ent.vel = vscale(player->dir, 3.5f);
                game.puck.owner = NULL;
            } else if (pass) {
                player_t *target_player = NULL;
                float target_angle = 0.0f;
                vec2_t target_dir;

                for (int j = 0; j < PLAYER_COUNT; ++j) {
                    if (i == j) {
                        continue;
                    }

                    player_t *other = &team->players[j];
                    vec2_t to_other = vnormalized(vsub(other->ent.pos, player->ent.pos));
                    float angle = vdot(player->dir, to_other);

                    if (angle > target_angle) {
                        target_player = other;
                        target_angle = angle;
                        target_dir = to_other;
                    }
                }

                if (target_player != NULL) {
                    game.puck.ent.vel = vscale(target_dir, 2.0f);
                    game.puck.owner = NULL;
                } else {
                    game.puck.ent.vel = vscale(player->dir, 2.0f);
                    game.puck.owner = NULL;
                }
            }
        }
    }    
}

static void update_game(void) {
    update_puck();
    update_team(&game.teams[0], *GAMEPAD1);
}


static void update_camera(void) {
    int x = screen(game.teams[0].players[game.teams[0].active_player].ent.pos.x);


    int camera_diff = x - game.camera;
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
}

static void draw_player(player_t *player, int team) {
    *DRAW_COLORS = 0x40 | (team == 0 ? 0x02 : 0x03);
    oval(screen(player->ent.pos.x) - 4 - game.camera, screen(player->ent.pos.y) - 4, 8, 8);
}

static void draw(void) {
    *DRAW_COLORS = 0x4321;
    blit(rink, -game.camera, 0, rinkWidth, rinkHeight, rinkFlags);
    blit(rink, -game.camera + SCREEN_SIZE, 0, rinkWidth, rinkHeight, rinkFlags|BLIT_FLIP_X);


    for (int i = 0; i < PLAYER_COUNT; ++i) {
        draw_player(&game.teams[0].players[i], 0);
    }

    draw_puck();
}



static void new_game(void) {
    memset(&game, 0, sizeof(game_t));
    game.camera = SCREEN_SIZE / 2;

    // Puck
    game.puck.ent.pos = vec(140, 87);

    game.puck.ent.vel.x = 1.0f;
    game.puck.ent.vel.y = 1.0f;
    game.puck.ent.size = 4.0f;

    game.teams[0].active_player = PLAYER_ATTACKER1;

    for (int i = 0; i < PLAYER_COUNT; ++i) {
        game.teams[0].players[i].ent.pos = player_lineup[i];
        game.teams[0].players[i].ent.size = 4.0f;
    }
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
