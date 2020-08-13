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

SPRITE* sprite_create(int w, int h)
{
    SPRITE *sprite = NULL;
    if (w <= 0 || h <= 0) return NULL;
    sprite = calloc(1, sizeof(SPRITE) + w * h * 2);
    if (!sprite) return NULL;
    sprite->w    = w;
    sprite->h    = h;
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
    sprite = sprite_create(w, h);
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

void sprite_destroy(SPRITE *sprite)
{
    if (sprite) free(sprite);
}
