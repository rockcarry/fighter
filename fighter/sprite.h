#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "platform.h"

typedef struct tagSPRITE {
    int32_t  x;
    int32_t  y;
    int32_t  w;
    int32_t  h;
    int32_t  vx;
    int32_t  vy;
    int32_t  ax;
    int32_t  ay;
    uint8_t *data;
    struct tagSPRITE *prev;
    struct tagSPRITE *next;
} SPRITE;

SPRITE* sprite_create (int w, int h);
SPRITE* sprite_load   (char *file);
int     sprite_save   (char *file, SPRITE *sprite);
void    sprite_destroy(SPRITE *sprite);

#endif
