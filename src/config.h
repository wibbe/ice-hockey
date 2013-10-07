
#ifndef _CONFIG_H_
#define _CONFIG_H_

//=============================================================================
#ifdef _WINDOWS

  #pragma pack(1)
  #pragma warning(disable:4996) // Using open/close/read... for file access
  #define _CRT_NONSTDC_NO_DEPRECATE
  #include <SDKDDKVer.h>
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <ShlObj.h>
  #include <gl/gl.h>
  #include <GL/glu.h>
  #include <al.h>
  #include <alc.h>
  #include <io.h>

//=============================================================================
#elif defined(__APPLE__)

  #if defined(__MACH__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
    #include <unistd.h>
    #include <GL/glfw.h>
    #define GL_BGRA_EXT GL_BGRA
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
  #endif
#else
  #include <GL/glfw.h>
  #include <AL/al.h>
  #include <AL/alc.h>
  #include <unistd.h>
#endif

//=============================================================================
// Common includes

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>

#endif