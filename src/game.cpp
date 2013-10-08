
#include "config.h"
#include "base.h"
#include "sys.h"
#include "core.h"

double g_time;

#define RINK_WIDTH  61.0
#define RINK_HEIGHT 30.5

// -- Textures --

enum TexID
{
  TEXTURE_RINK = 0
};

struct Texture
{
  const char * file;
  int tex;
} TEXTURES[] = {
  { "data/rink.bmp", 0 }
};

void loadTextures()
{
  for (int i = 0; i < SIZE_ARRAY(TEXTURES); ++i)
    TEXTURES[i].tex = CORE_LoadBmp(TEXTURES[i].file, false);
}

void unloadTextures()
{
  for (int i = 0; i < SIZE_ARRAY(TEXTURES); ++i)
    CORE_UnloadBmp(TEXTURES[i].tex);
}

int texId(int id)
{
  assert(id < SIZE_ARRAY(TEXTURES));
  return TEXTURES[id].tex;
}

// -- Input --

enum InputMask
{
  INPUT_LEFT = 0,
  INPUT_RIGHT = 1,
  INPUT_UP = 2,
  INPUT_DOWN = 4,
  INPUT_SHOOT = 8,
  INPUT_PASS = 16
};

struct InputMapping
{
  int key;
  int bit;
} INPUT_MAPPINGS[2][4] = {
  { // Player 1
    { SYS_KEY_LEFT, INPUT_LEFT },
    { SYS_KEY_RIGHT, INPUT_RIGHT },
    { SYS_KEY_UP, INPUT_UP },
    { SYS_KEY_DOWN, INPUT_DOWN }
  },
  { // Player 2
    { SYS_KEY_LEFT, INPUT_LEFT },
    { SYS_KEY_RIGHT, INPUT_RIGHT },
    { SYS_KEY_UP, INPUT_UP },
    { SYS_KEY_DOWN, INPUT_DOWN }
  }
};

// -- Entities --

#define MAX_ENTITIES 256

// -- Components --

struct Position
{
  vec2 pos;
};

struct Velocity
{
  vec2 vel;
};

struct Input
{
  unsigned char input;
};

struct Team
{
  int players[5];
  short score;
  char selected;
};

struct Sprite
{
  vec2 size;
  int tex;
  rgba color;
};

struct Collider
{
  float bounce;
  float radius;
};

enum COMPONENTS
{
  COMPONENT_NONE = 0,
  COMPONENT_POSITION = 1 << 0,
  COMPONENT_VELOCITY = 1 << 1,
  COMPONENT_INPUT = 1 << 2,
  COMPONENT_TEAM = 1 << 3,
  COMPONENT_SPRITE = 1 << 4,
  COMPONENT_COLLIDER = 1 << 5
};

// -- World --

struct World
{
  int mask[MAX_ENTITIES];

  Position position[MAX_ENTITIES];
  Velocity velocity[MAX_ENTITIES];
  Input input[MAX_ENTITIES];
  Team team[MAX_ENTITIES];
  Sprite sprite[MAX_ENTITIES];
  Collider collider[MAX_ENTITIES];
};

void initWorld(World & world)
{
  for (int i = 0; i < MAX_ENTITIES; ++i)
    world.mask[i] = COMPONENT_NONE;
}

int createEntity(World & world, int componentMask)
{
  for (int i = 0; i < MAX_ENTITIES; ++i)
  {
    if (world.mask[i] == COMPONENT_NONE)
    {
      world.mask[i] = componentMask;
      return i;
    }
  }

  return -1;
}

void destroyEntity(World & world, int entity)
{
  assert(entity < MAX_ENTITIES);
  world.mask[entity] = 0;
}

#define FOR_ENTITY(components) \
  const int MASK = components; \
  for (int i = 0; i < MAX_ENTITIES; ++i) \
    if ((g_world.mask[i] & MASK) == MASK)

// -- Main Game

World g_world;
int g_team1;
int g_team2;

// -- Systems --

void moveObjects(double dt)
{
  FOR_ENTITY(COMPONENT_POSITION | COMPONENT_VELOCITY)
  {
    Position & pos = g_world.position[i];
    Velocity & vel = g_world.velocity[i];

    pos.pos = vadd(pos.pos, vscale(vel.vel, dt));
  }
}

void checkCollisions()
{
  const double halfWidth = RINK_WIDTH * 0.5;
  const double halfHeight = RINK_HEIGHT * 0.5;

  // Check against the walls
  FOR_ENTITY(COMPONENT_POSITION | COMPONENT_VELOCITY | COMPONENT_COLLIDER)
  {
    Position & pos = g_world.position[i];
    Velocity & vel = g_world.velocity[i];
    Collider & col = g_world.collider[i];

    if ((pos.pos.x + col.radius) > halfWidth || (pos.pos.x - col.radius) < -halfWidth)
    {
      vel.vel.x *= -col.bounce;
    }
    else if ((pos.pos.y + col.radius) > halfHeight || (pos.pos.y - col.radius) < -halfHeight)
    {
      vel.vel.y *= -col.bounce;
    }
  }
}

// -- Main Game

int createTeam()
{
  int team = createEntity(g_world, COMPONENT_TEAM | COMPONENT_INPUT);

  g_world.team[team].score = 0;
  g_world.team[team].selected = -1;
  g_world.team[team].players[0] = -1;
  g_world.team[team].players[1] = -1;
  g_world.team[team].players[2] = -1;
  g_world.team[team].players[3] = -1;
  g_world.team[team].players[4] = -1;
  g_world.input[team].input = 0;

  return team;
}

int createRink()
{
  int rink = createEntity(g_world, COMPONENT_POSITION | COMPONENT_SPRITE);

  g_world.position[rink].pos = vmake(0, 0);
  g_world.sprite[rink].size = vmake(RINK_WIDTH, RINK_HEIGHT);
  g_world.sprite[rink].color = COLOR_WHITE;
  g_world.sprite[rink].tex = texId(TEXTURE_RINK);

  return rink;
}

void startGame()
{
  loadTextures();

  initWorld(g_world);

  g_team1 = createTeam();
  g_team2 = createTeam();

  createRink();

  g_time = 0.0;
}

void endGame()
{
  unloadTextures();
}

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  // Render all sprites
  FOR_ENTITY(COMPONENT_POSITION | COMPONENT_SPRITE)
  {
    Position & pos = g_world.position[i];
    Sprite & sprite = g_world.sprite[i];

    printf("Render sprite %d\n", sprite.tex);
    CORE_RenderCenteredSprite(pos.pos, sprite.size, sprite.tex, sprite.color);
  }
}

void processInput()
{
  Input & input1 = g_world.input[g_team1];
  Input & input2 = g_world.input[g_team2];

  input1.input = 0;
  input2.input = 0;

  for (int i = 0; i < SIZE_ARRAY(INPUT_MAPPINGS[0]); ++i)
  {
    input1.input |= SYS_KeyPressed(INPUT_MAPPINGS[0][i].key) ? INPUT_MAPPINGS[0][i].bit : 0;
    input1.input |= SYS_KeyPressed(INPUT_MAPPINGS[1][i].key) ? INPUT_MAPPINGS[1][i].bit : 0;
  }
}

void runGame()
{
  const double dt = 1.0 / 60.0;

  moveObjects(dt);
  checkCollisions();
}

int start()
{
  startGame();

  glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
  glClearColor(0.0f, 0.1f, 0.3f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (SYS_WIDTH / SYS_HEIGHT) * G_HEIGHT, 0.0, G_HEIGHT, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef((SYS_WIDTH / SYS_HEIGHT) * G_HEIGHT * 0.5, G_HEIGHT * 0.5, 0.0);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (!SYS_GottaQuit())
  {
    render();
    SYS_Show();
    processInput();
    runGame();
    SYS_Pump();
    SYS_Sleep(16);
    g_time += 1.0 / 60.0;
  }

  endGame();

  return 0;
}