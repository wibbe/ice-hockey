
#ifndef PHYSICS_H
#define PHYSICS_H

#include "vec2.h"

typedef struct line_t {
    uint16_t start;
    uint16_t end;
} line_t;

typedef struct static_collider_t {
    vec2_t *points;
    line_t *lines;
    int lines_count;
} static_collider_t;

typedef struct collision_t {
	bool collide;
	float force;
	vec2_t normal;
} collision_t;

typedef struct entity_t {
    vec2_t pos;
    vec2_t vel;
    float size;
    float mass;
} entity_t;


collision_t static_collide_entity(entity_t *ent, static_collider_t *collider);
collision_t dynamic_collide_entity(entity_t *a, entity_t *b);
void simulate_entity(entity_t* ent);

#endif


#ifdef PHYSICS_IMPLEMENTATION

collision_t static_collide_entity(entity_t *ent, static_collider_t *collider) {
    collision_t collision = {0};

    for (int i = 0; i < collider->lines_count; ++i) {
        line_t line = collider->lines[i];
        vec2_t start = collider->points[line.start];
        vec2_t end = collider->points[line.end];

        float line_len = vlength(vsub(end, start));
        vec2_t line_dir = vscale(vsub(end, start), 1.0f / line_len);
        vec2_t line_norm = vperp(line_dir);
        vec2_t to_ent = vsub(ent->pos, start);

        vec2_t point;

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

            collision.collide = true;
            float force = vdot(ent->vel, vscale(line_norm, -1.0f));
            if (collision.force < force) {
            	collision.force = force;
            	collision.normal = line_norm;
            }

            ent->pos = vadd(ent->pos, vscale(line_norm, overlap));
            ent->vel = vscale(vreflect(ent->vel, line_norm), 1.0f);
        }
    }

    return collision;
}

collision_t dynamic_collide_entity(entity_t *a, entity_t *b) {
	collision_t collision = {0};

	vec2_t to_b = vsub(b->pos, a->pos);
	float separation = vlength(to_b);
	vec2_t normal = vscale(to_b, 1.0f / separation);

	if (separation < (a->size + b->size)) {
		float overlap = (a->size + b->size) - separation;

		// Move the two entities away from each other so they no longer collide
		a->pos = vadd(a->pos, vscale(normal, overlap * -0.5f));
		b->pos = vadd(b->pos, vscale(normal, overlap * 0.5f));

		// Reflect their velocities
		a->vel = vreflect(a->vel, vscale(normal, -1.0f));
		b->vel = vreflect(b->vel, normal);

		collision.collide = true;
	}

	return collision;
}

void simulate_entity(entity_t* ent) {
    ent->pos = vadd(ent->pos, ent->vel);

    float speed = vlength(ent->vel);
    vec2_t dir = vscale(ent->vel, 1.0f / speed);

    vec2_t friction = vscale(dir, speed * -0.005f);
    //ent->vel = vadd(ent->vel, friction);

    if (vdot(ent->vel, dir) < 0.0f) {
        ent->vel = vzero();
    }
}

#undef PHYSICS_IMPLEMENTATION
#endif