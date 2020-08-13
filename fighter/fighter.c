#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "platform.h"
#include "sprite.h"

void render_game(SPRITE *sprite)
{
    int i, j;

    // draw background
    for (i=0; i<SCREEN_HEIGHT; i++) {
        for (j=0; j<SCREEN_WIDTH; j++) {
            tile(j, i, 0x0F, ' ');
        }
    }

    // draw sprite
    sprite_run (sprite, -1);
    sprite_draw(sprite, -1);
}

int main(void)
{
    int      sleeptick = 0, quit = 0, i;
    uint32_t next_tick = 0;
    SPRITE  *sprite    = NULL;

    platform_init();
    sprite = sprite_load("sprite.ini");
    for (i=0; i<6; i++) {
        SPRITE *s = sprite_new(1, 1);
        s->data[0] = 'A' + rand() % 26;
        s->data[1] = 0x0F;
        s->x       = rand() % SCREEN_WIDTH ;
        s->y       = rand() % SCREEN_HEIGHT;
        s->vx      = rand();
        s->vy      = rand();
        sprite_insert(sprite, s);
    }

    while (!quit) {
        render_game(sprite); vpost(); // render game

        // handle input
        if (getkey('Q') || getkey('q')) {
            quit = 1; 
        } else if (getkey('E') || getkey('e')) {
            if (sprite->y > 0) sprite->y--;
        } else if (getkey('D') || getkey('d')) {
            if (sprite->y < SCREEN_HEIGHT - sprite->h) sprite->y++;
        } else if (getkey('S') || getkey('s')) {
            if (sprite->x > 0) sprite->x--;
        } else if (getkey('F') || getkey('f')) {
            if (sprite->x < SCREEN_WIDTH  - sprite->w) sprite->x++;
        }

        // frame rate control
        next_tick = (next_tick ? next_tick : get_tick_count()) + 20;
        sleeptick = next_tick - get_tick_count();
        if (sleeptick > 0) usleep(sleeptick);
    }

    sprite_free(sprite, -1);
    platform_exit();
    return 0;
}
