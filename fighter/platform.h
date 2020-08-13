#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef WIN32
#include <windows.h>

// disable warnings
#pragma warning(disable:4996)

#define usleep(t)       Sleep((t) / 1000)
#define get_tick_count  GetTickCount

typedef signed   char   int8_t;
typedef unsigned char  uint8_t;
typedef signed   short  int16_t;
typedef unsigned short uint16_t;
typedef signed   int    int32_t;
typedef unsigned int   uint32_t;
#endif

#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25

void platform_init(void);
void platform_exit(void);

void tile  (int x, int y, int c, int t);
void vpost (void);
int  getkey(int key);

#endif
