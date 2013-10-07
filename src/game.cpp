
#include "config.h"
#include "base.h"
#include "sys.h"
#include "core.h"

double g_time;

void startGame()
{
  g_time = 0.0;
}

void endGame()
{

}

void render()
{

}

void processInput()
{

}

void runGame()
{

}

int start()
{
  startGame();

  glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
  glClearColor(0.0f, 0.1f, 0.3f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, G_WIDTH, 0.0, G_HEIGHT, 0.0, 1.0);
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
    g_time += 1.f/60.f;
  }

  endGame();

  return 0;
}