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
    int32_t  tx;
    int32_t  ty;
    int32_t  bound_left;
    int32_t  bound_top;
    int32_t  bound_right;
    int32_t  bound_bottom;
    uint8_t *data;
    #define SPRITE_COLLISION_REBOUND  (1 << 0)
    #define SPRITE_COLLISION_DESTROY  (1 << 1)
    uint32_t flags;
    struct tagSPRITE *prev;
    struct tagSPRITE *next;
} SPRITE;

SPRITE* sprite_new   (int w, int h);
SPRITE* sprite_load  (char *file);
int     sprite_save  (char *file, SPRITE *sprite);

int     sprite_free  (SPRITE *sprite, int n);
int     sprite_draw  (SPRITE *sprite, int n);
int     sprite_run   (SPRITE *sprite, int n);

void    sprite_insert(SPRITE *sprite1, SPRITE *sprite2);
void    sprite_remove(SPRITE *sprite , int f);

#endif
