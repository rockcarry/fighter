#include <windows.h>
#include "platform.h"

static BYTE g_screen_buf[SCREEN_WIDTH * SCREEN_HEIGHT * 3] = {0};

void tile(int x, int y, int c, int t)
{
    if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    ((WORD*)g_screen_buf)[y * SCREEN_WIDTH + x] = c;
    g_screen_buf[SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(WORD) + y * SCREEN_WIDTH + x] = t;
}

void vpost(void)
{
    HANDLE h  = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD loc = {0};
    WriteConsoleOutputCharacterA(h, g_screen_buf + SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(WORD), SCREEN_WIDTH * SCREEN_HEIGHT, loc, NULL);
    WriteConsoleOutputAttribute (h, (WORD*)g_screen_buf, SCREEN_WIDTH * SCREEN_HEIGHT, loc, NULL);
}

static HANDLE g_hKeyEventThread    = NULL;
static DWORD  g_dwKeyEventFlags[8] = {0};
static BOOL   g_bExitKeyThread     = FALSE;
static WINAPI KeyEventThreadProc(LPVOID lpThreadParameter)
{
    INPUT_RECORD irec;
    DWORD        cc;
    while (!g_bExitKeyThread) {
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &irec, 1, &cc);
        if (irec.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD *key = (KEY_EVENT_RECORD*)&irec.Event;
            int n = key->wVirtualKeyCode / 32;
            int m = key->wVirtualKeyCode % 32;
            if (key->bKeyDown) {
                if (key->wVirtualKeyCode > 0 && key->wVirtualKeyCode < 0xFF) {
                    g_dwKeyEventFlags[n] |= (1 << m);
                }
            } else {
                if (key->wVirtualKeyCode > 0 && key->wVirtualKeyCode < 0xFF) {
                    g_dwKeyEventFlags[n] &=~(1 << m);
                }
            }
        }
    }
    return 0;
}

static void input_init(void)
{
    if (g_hKeyEventThread == NULL) {
        g_bExitKeyThread  = FALSE;
        g_hKeyEventThread = CreateThread(NULL, 0, KeyEventThreadProc, NULL, 0, NULL);
    }
}

static void input_exit(void)
{
    if (g_hKeyEventThread != NULL) {
        g_bExitKeyThread = TRUE;
        WaitForSingleObject(g_hKeyEventThread, -1);
        CloseHandle(g_hKeyEventThread);
        g_hKeyEventThread = NULL;
    }
}

static int input_getkey(int key)
{
    int n = key / 32;
    int m = key % 32;
    return !!(g_dwKeyEventFlags[n] & (1 << m));
}

int getkey(int key)
{
    return input_getkey(key);
}

void platform_init(void)
{
    SMALL_RECT rect = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1 };
    COORD     coord = { SCREEN_WIDTH, SCREEN_HEIGHT };
    CONSOLE_CURSOR_INFO info = {1};
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    input_init();
}

void platform_exit(void)
{
    input_exit();
}

