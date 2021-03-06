
#ifndef _SYS_H_
#define _SYS_H_

//========================================================================================
// Resolution (real & virtual)

// Proportional to iPhone for Mac: 512x768 (it's 80% of 640x960, which is iPhone Retina)
#define SYS_WIDTH  1024
#define SYS_HEIGHT 768
#define SYS_FULLSCREEN 0

// Main game coordinate system: width is 1000, height is 1500. Origin center of screen
#define G_HEIGHT 40.0

//========================================================================================
// Platform layer

void  SYS_Pump();
void  SYS_Show();
bool  SYS_GottaQuit();
void  SYS_Sleep(int ms);
bool  SYS_KeyPressed(int key);
ivec2 SYS_MousePos();
bool  SYS_MouseButonPressed(int button);
int   SYS_OpenConfigFile(bool write);

#ifdef _WINDOWS

#define SYS_KEY_UP    VK_UP
#define SYS_KEY_DOWN  VK_DOWN
#define SYS_KEY_LEFT  VK_LEFT
#define SYS_KEY_RIGHT VK_RIGHT
#define SYS_KEY_ENTER VK_RETURN
#define SYS_KEY_ESC   VK_ESCAPE
#define SYS_KEY_SPACE VK_SPACE

#define SYS_MB_LEFT   VK_LBUTTON
#define SYS_MB_RIGHT  VK_RBUTTON
#define SYS_MB_MIDDLE VK_MBUTTON

#else

#define SYS_KEY_UP    GLFW_KEY_UP
#define SYS_KEY_DOWN  GLFW_KEY_DOWN
#define SYS_KEY_LEFT  GLFW_KEY_LEFT
#define SYS_KEY_RIGHT GLFW_KEY_RIGHT
#define SYS_KEY_ENTER GLFW_KEY_ENTER
#define SYS_KEY_ESC   GLFW_KEY_ESC
#define SYS_KEY_SPACE GLFW_KEY_SPACE

#define SYS_MB_LEFT   GLFW_MOUSE_BUTTON_LEFT
#define SYS_MB_RIGHT  GLFW_MOUSE_BUTTON_RIGHT
#define SYS_MB_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE

#endif


#endif
