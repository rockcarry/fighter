#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "platform.h"

void render_game(int x, int y, uint8_t *sprite)
{
    int sw, sh, tx, ty, i, j;

    // draw background
    for (i=0; i<SCREEN_HEIGHT; i++) {
        for (j=0; j<SCREEN_WIDTH; j++) {
            tile(j, i, 0x0F, ' ');
        }
    }

    // draw sprite
    sw = sprite[0];
    sh = sprite[1];
    for (i=0; i<sh; i++) {
        for (j=0; j<sw; j++) {
            tx = x + j; ty = y + i;
            if (tx < SCREEN_WIDTH && ty < SCREEN_HEIGHT && sprite[2 + i * sw + j] != ' ') {
                tile(tx, ty, sprite[2 + sw * sh + i * sw + j], sprite[2 + i * sw + j]);
            }
        }
    }
}

int main(void)
{
    int quit = 0, x = SCREEN_WIDTH/2, y = SCREEN_HEIGHT-2, sleeptick;
    uint32_t next_tick = 0;
    uint8_t sprite[2 + 2 * 3 * 2] = {
        3, 2,
        ' ', '#', ' ',
        'A', '@', 'A',
        0x00, 0x0F, 0x00,
        0x0A, 0x0E, 0x0A,
    };

    platform_init();

    while (!quit) {
        render_game(x, y, sprite); vpost(); // render game

        // handle input
        if (getkey('Q') || getkey('q')) {
            quit = 1; 
        } else if (getkey('E') || getkey('e')) {
            if (y > 0) y--;
        } else if (getkey('D') || getkey('d')) {
            if (y < SCREEN_HEIGHT - sprite[1]) y++;
        } else if (getkey('S') || getkey('s')) {
            if (x > 0) x--;
        } else if (getkey('F') || getkey('f')) {
            if (x < SCREEN_WIDTH - sprite[0]) x++;
        }

        // frame rate control
        next_tick = (next_tick ? next_tick : get_tick_count()) + 20;
        sleeptick = next_tick - get_tick_count();
        if (sleeptick > 0) usleep(sleeptick);
    }

    platform_exit();
    return 0;
}
