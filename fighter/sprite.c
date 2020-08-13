#include <stdlib.h>
#include <stdio.h>
#include "sprite.h"

static char* parse_params(const char *str, const char *key, char *val, int len)
{
    char *p = (char*)strstr(str, key);
    int   i;

    if (!p) return NULL;
    p += strlen(key);
    if (*p == '\0') return NULL;

    while (1) {
        if (*p != ' ' && *p != ':') break;
        else p++;
    }

    for (i=0; i<len; i++) {
        if (*p == '\\') {
            p++;
        } else if (*p == '\n' || *p == '\0') {
            break;
        }
        val[i] = *p++;
    }
    val[i] = val[len-1] = '\0';
    return val;
}

static char* str_get_token(const char *str, char *token, int len)
{
    char *p = (char*)str;
    int   i;

    while (*p && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')) p++;
    for (i=0; i<len && *p; i++) {
        if (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t') break;
        token[i] = *p++;
    }
    token[i] = token[len-1] = '\0';
    return p;
}

SPRITE* sprite_new(int w, int h)
{
    SPRITE *sprite = NULL;
    if (w <= 0 || h <= 0) return NULL;
    sprite = calloc(1, sizeof(SPRITE) + w * h * 2);
    if (!sprite) return NULL;
    sprite->w = w;
    sprite->h = h;
    sprite->flags       |= SPRITE_COLLISION_REBOUND;
    sprite->bound_right  = SCREEN_WIDTH  - 1;
    sprite->bound_bottom = SCREEN_HEIGHT - 1;
    sprite->data = (uint8_t*)sprite + sizeof(SPRITE);
    memset(sprite->data, ' ', w * h);
    return sprite;
}

SPRITE* sprite_load(char *file)
{
    SPRITE *sprite = NULL;
    char   *fdata  = NULL;
    int     fsize  = 0, w, h, i, j;
    char    str[32], *p;

    FILE *fp = fopen(file, "rb");
    if (!fp) goto done;

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fdata = malloc(fsize + 1);
    if (!fdata) goto done;
    fread(fdata, 1, fsize, fp);
    fdata[fsize] = '\0';

    parse_params(fdata, "width" , str, sizeof(str)); w = atoi(str);
    parse_params(fdata, "height", str, sizeof(str)); h = atoi(str);
    sprite = sprite_new(w, h);
    if (!sprite) goto done;

    p = strstr(fdata, "data"); while (*p && *p != '\n') p++;
    if (*++p == '\0') goto done;
    for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
            sprite->data[i * w + j] = *p++;
            if (*p == '\0') goto done;
            if (*p == '\n') break;
        }
        if (*p != '\n') {
            while (*p && *p != '\n') p++;
            if (*p == '\0') goto done;
        }
        if (*++p == '\0') goto done;
    }

    for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
            p = str_get_token(p, str, sizeof(str));
            sprite->data[w * h + i * w + j] = (uint8_t)strtoul(str, NULL, 16);
        }
    }

done:
    if (fdata) free(fdata);
    if (fp) fclose(fp);
    return sprite;
}

int sprite_save(char *file, SPRITE *sprite)
{
    int i, j;
    FILE *fp = fopen(file, "wb");
    if (!fp) return -1;
    fprintf(fp, "width : %d\r\n", sprite->w);
    fprintf(fp, "height: %d\r\n", sprite->h);
    fprintf(fp, "\r\n");
    fprintf(fp, "data:\r\n");
    for (i=0; i<sprite->h; i++) {
        for (j=0; j<sprite->w; j++) {
            fprintf(fp, "%c", sprite->data[i * sprite->w + j]);
        }
        fprintf(fp, "\r\n");
    }
    fprintf(fp, "\r\n");
    for (i=0; i<sprite->h; i++) {
        for (j=0; j<sprite->w; j++) {
            fprintf(fp, "%02x ", sprite->data[sprite->w * sprite->h + i * sprite->w + j]);
        }
        fprintf(fp, "\r\n");
    }
    fclose(fp);
    return 0;
}

int sprite_free(SPRITE *sprite, int n)
{
    int k = 0;
    while (sprite && ++k != n) {
        SPRITE *next = sprite->next;
        sprite_remove(sprite, 1);
        sprite = next;
    }
    return k;
}

int sprite_draw(SPRITE *sprite, int n)
{
    int i, j, k = 0;
    while (sprite && ++k != n) {
        for (i=0; i<sprite->h; i++) {
            for (j=0; j<sprite->w; j++) {
                int x = sprite->x + j;
                int y = sprite->y + i;
                if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT && sprite->data[i * sprite->w + j] != ' ') {
                    tile(x, y, sprite->data[sprite->w * sprite->h + i * sprite->w + j], sprite->data[i * sprite->w + j]);
                }
            }
        }
        sprite = sprite->next;
    }
    return k;
}

void sprite_insert(SPRITE *sprite1, SPRITE *sprite2)
{
    if (sprite1->next) sprite1->next->prev = sprite2;
    sprite2->next = sprite1->next;
    sprite2->prev = sprite1;
    sprite1->next = sprite2;
}

void sprite_remove(SPRITE *sprite, int f)
{
    if (sprite->next) sprite->next->prev = sprite->prev;
    if (sprite->prev) sprite->prev->next = sprite->next;
    if (f) free(sprite);
}

int sprite_run(SPRITE *sprite, int n)
{
    int k = 0;
    while (sprite && ++k != n) {
        sprite->tx += sprite->vx;
        sprite->ty += sprite->vy;
        if (abs(sprite->tx) >= (1 << 16)) {
            sprite->x += sprite->tx > 0 ? 1 : -1;
            sprite->tx-= sprite->tx > 0 ? (1 << 16) : -(1 << 16);
        }
        if (abs(sprite->ty) >= (1 << 17)) {
            sprite->y += sprite->ty > 0 ? 1 : -1;
            sprite->ty-= sprite->ty > 0 ? (1 << 17) : -(1 << 17);
        }
        sprite->vx += sprite->ax;
        sprite->vy += sprite->ay;
        if (sprite->vx > 0x40000000) sprite->vx = 0x40000000;
        if (sprite->vx <-0x40000000) sprite->vx =-0x40000000;
        if (sprite->vy > 0x40000000) sprite->vy = 0x40000000;
        if (sprite->vy <-0x40000000) sprite->vy =-0x40000000;
        if (sprite->flags & SPRITE_COLLISION_REBOUND) {
            if (sprite->x <= sprite->bound_left && sprite->tx <= 0 || sprite->x > sprite->bound_right  && sprite->tx > 0) sprite->vx = -sprite->vx;
            if (sprite->y <= sprite->bound_top  && sprite->ty <= 0 || sprite->y > sprite->bound_bottom && sprite->ty > 0) sprite->vy = -sprite->vy;
        }
        if (sprite->flags & SPRITE_COLLISION_DESTROY) {
            if (  sprite->x < sprite->bound_left || sprite->x > sprite->bound_right
               || sprite->y < sprite->bound_top  || sprite->y > sprite->bound_bottom) {
                SPRITE *prev = sprite->prev;
                sprite_remove(sprite, 1);
                sprite = prev;
            }
        }
        if (sprite) sprite = sprite->next;
    }
    return k;
}
