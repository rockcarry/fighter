#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "platform.h"
#include "sprite.h"

void render_game(SPRITE *sprite)
{
    int x, y, i, j;

    // draw background
    for (i=0; i<SCREEN_HEIGHT; i++) {
        for (j=0; j<SCREEN_WIDTH; j++) {
            tile(j, i, 0x0F, ' ');
        }
    }

    // draw sprite
    for (i=0; i<sprite->h; i++) {
        for (j=0; j<sprite->w; j++) {
            x = sprite->x + j; y = sprite->y + i;
            if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT && sprite->data[i * sprite->w + j] != ' ') {
                tile(x, y, sprite->data[sprite->w * sprite->h + i * sprite->w + j], sprite->data[i * sprite->w + j]);
            }
        }
    }
}

int main(void)
{
    int      sleeptick = 0, quit = 0;
    uint32_t next_tick = 0;
    SPRITE  *sprite    = NULL;

    platform_init();
    sprite = sprite_load("sprite.ini");

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

    sprite_destroy(sprite);
    platform_exit();
    return 0;
}
