#include "naga8_api.h"
#include "naga8_conf.h"
#include "raylib.h"
#include "naga_font3x4.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned char g_fb[NAGA8_W * NAGA8_H];
static unsigned char g_rgba[NAGA8_W * NAGA8_H * 4];
static RenderTexture2D g_rt;
static bool g_inited = false;

static const Color g_pal[16] = {
    /*  0 Black     */ {   0,   0,   0, 255 },
    /*  1 Navy      */ {  43,  51,  95, 255 },
    /*  2 Purple    */ { 126,  32, 114, 255 },
    /*  3 Green     */ {  25, 149, 156, 255 },
    /*  4 Brown     */ { 139,  72,  82, 255 },
    /*  5 Dark Blue */ {  57,  92, 152, 255 },
    /*  6 Light Blue*/ { 169, 193, 255, 255 },
    /*  7 White     */ { 238, 238, 238, 255 },
    /*  8 Red       */ { 212,  24, 108, 255 },
    /*  9 Orange    */ { 211, 132,  65, 255 },
    /* 10 Yellow    */ { 233, 195,  91, 255 },
    /* 11 Lime      */ { 112, 198, 169, 255 },
    /* 12 Cyan      */ { 118, 150, 222, 255 },
    /* 13 Gray      */ { 163, 163, 163, 255 },
    /* 14 Pink      */ { 255, 151, 152, 255 },
    /* 15 Peach     */ { 237, 199, 176, 255 },
};

typedef struct {
    int w, h;
    unsigned char* data;
} NagaSprite;

static NagaSprite g_sprites[256];

static int g_fps_value  = 0;
static int g_fps_frames = 0;
static time_t g_fps_last = 0;
static bool g_show_fps = false;

void naga8_fps_tick(void) {
    g_fps_frames++;
    time_t now = time(NULL);
    if (g_fps_last == 0) g_fps_last = now;
    if (now - g_fps_last >= 1) {
        g_fps_value  = g_fps_frames;
        g_fps_frames = 0;
        g_fps_last   = now;
    }
}

int naga8_fps_get(void) {
    return g_fps_value;
}

void naga8_poll(void)
{
    if (!g_inited) return;

    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

    // Ctrl + P — переключение FPS
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_P)) {
        g_show_fps = !g_show_fps;
    }
}

void naga8_init(void)
{
    if (g_inited) return;
    
    InitWindow(NAGA8_W * NAGA8_SCALE, NAGA8_H * NAGA8_SCALE, "NAGA-8");
    
    // Включаем ресайз уже после создания окна — как в старом коде
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(60);
    
    g_rt = LoadRenderTexture(NAGA8_W, NAGA8_H);
    SetTextureFilter(g_rt.texture, TEXTURE_FILTER_POINT);
    
    for (int i = 0; i < NAGA8_W * NAGA8_H; i++) g_fb[i] = 0;
    g_inited = true;
}

void naga8_close(void)
{
    if (!g_inited) return;
    UnloadRenderTexture(g_rt);
    CloseWindow();
    g_inited = false;
}

void naga8_cls(int color)
{
    unsigned char c = (unsigned char)(color & 15);
    for (int i = 0; i < NAGA8_W * NAGA8_H; i++) g_fb[i] = c;
}

void naga8_pset(int x, int y, int color)
{
    if (x < 0 || x >= NAGA8_W || y < 0 || y >= NAGA8_H) return;
    g_fb[y * NAGA8_W + x] = (unsigned char)(color & 15);
}

int naga8_pget(int x, int y)
{
    if (x < 0 || x >= NAGA8_W || y < 0 || y >= NAGA8_H) return 0;
    return g_fb[y * NAGA8_W + x] & 15;
}

void naga8_text(const char* s, int x, int y, int color)
{
    if (!s) return;

    unsigned char c = (unsigned char)(color & 15);
    int cx = x, cy = y;

    for (const unsigned char* p = (const unsigned char*)s; *p; p++) {
        if (*p == '\n') { cx = x; cy += FONT_CELL_H + 1; continue; }
        if (*p == '\r') continue;

        unsigned char u = *p;
        if (u >= 'a' && u <= 'z') u -= 32;   /* шрифт только заглавный */

        int idx = (int)u - FONT_FIRST;
        if (idx < 0 || idx >= FONT_COUNT) idx = '?' - FONT_FIRST;

        int w = naga8_font_w[idx];
        for (int row = 0; row < FONT_CELL_H; row++) {
            unsigned char bits = naga8_font[idx][row];
            for (int col = 0; col < FONT_CELL_W; col++) {
                if ((bits >> (FONT_CELL_W - 1 - col)) & 1) {
                    naga8_pset(cx + col, cy + (FONT_CELL_H - 1 - row), c);
                }
            }
        }
        cx += w + 1;   /* +1 — межбуквенный пробел */
    }
}

void naga8_rect(int x, int y, int w, int h, int color)
{
    if (w <= 0 || h <= 0) return;
    unsigned char c = (unsigned char)(color & 15);

    int x0 = x,            x1 = x + w;
    int y0 = y,            y1 = y + h;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > NAGA8_W) x1 = NAGA8_W;
    if (y1 > NAGA8_H) y1 = NAGA8_H;
    if (x0 >= x1 || y0 >= y1) return;

    for (int yy = y0; yy < y1; yy++) {
        for (int xx = x0; xx < x1; xx++) {
            g_fb[yy * NAGA8_W + xx] = c;
        }
    }
}

bool naga8_btn(int i, int p)
{
    if (p == 0) {
        switch (i) {
            case 0: return IsKeyDown(KEY_A);
            case 1: return IsKeyDown(KEY_D);
            case 2: return IsKeyDown(KEY_W);
            case 3: return IsKeyDown(KEY_S);
            case 4: return IsKeyDown(KEY_J);
            case 5: return IsKeyDown(KEY_K);
        }
    } else if (p == 1) {
        switch (i) {
            case 0: return IsKeyDown(KEY_LEFT);
            case 1: return IsKeyDown(KEY_RIGHT);
            case 2: return IsKeyDown(KEY_UP);
            case 3: return IsKeyDown(KEY_DOWN);
            case 4: return IsKeyDown(KEY_KP_4);
            case 5: return IsKeyDown(KEY_KP_5);
        }
    }
    return false;
}

bool naga8_btnp(int i, int p)
{
    if (p == 0) {
        switch (i) {
            case 0: return IsKeyPressed(KEY_A);
            case 1: return IsKeyPressed(KEY_D);
            case 2: return IsKeyPressed(KEY_W);
            case 3: return IsKeyPressed(KEY_S);
            case 4: return IsKeyPressed(KEY_J);
            case 5: return IsKeyPressed(KEY_K);
        }
    } else if (p == 1) {
        switch (i) {
            case 0: return IsKeyPressed(KEY_LEFT);
            case 1: return IsKeyPressed(KEY_RIGHT);
            case 2: return IsKeyPressed(KEY_UP);
            case 3: return IsKeyPressed(KEY_DOWN);
            case 4: return IsKeyPressed(KEY_KP_4);
            case 5: return IsKeyPressed(KEY_KP_5);
        }
    }
    return false;
}

void naga8_sprdata(int id, const char* data)
{
    if (id < 0 || id > 255 || !data) return;

    // Первый проход: ширина и высота (пустые строки от """ игнорируются)
    int h = 0, w = 0, cur_w = 0;
    for (const char* p = data; *p; p++) {
        if (*p == '\n') {
            if (cur_w > 0) {
                if (cur_w > w) w = cur_w;
                h++;
                cur_w = 0;
            }
        } else if (*p != '\r') {
            cur_w++;
        }
    }
    if (cur_w > 0) {
        if (cur_w > w) w = cur_w;
        h++;
    }
    if (w == 0 || h == 0) return;

    if (g_sprites[id].data) free(g_sprites[id].data);
    g_sprites[id].w = w;
    g_sprites[id].h = h;
    g_sprites[id].data = (unsigned char*)malloc(w * h);
    if (!g_sprites[id].data) return;

    // Второй проход: заполняем пиксели
    int row = 0, col = 0;
    for (const char* p = data; ; p++) {
        if (*p == '\n' || *p == '\0') {
            if (col > 0) row++;
            col = 0;
            if (*p == '\0' || row >= h) break;
        } else if (*p != '\r') {
            char c = *p;
            int color = 0;
            if      (c >= '0' && c <= '9') color = c - '0';
            else if (c >= 'a' && c <= 'f') color = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') color = c - 'A' + 10;
            if (row < h && col < w) {
                g_sprites[id].data[row * w + col] = (unsigned char)color;
            }
            col++;
        }
    }
}

void naga8_spr(int id, int x, int y, int flip_x, int flip_y)
{
    if (id < 0 || id > 255) return;
    NagaSprite* spr = &g_sprites[id];
    if (!spr->data) return;

    for (int row = 0; row < spr->h; row++) {
        for (int col = 0; col < spr->w; col++) {
            // Отражаем по X, если нужно
            int px = flip_x ? (spr->w - 1 - col) : col;
            
            // Читаем данные (всегда сверху вниз)
            unsigned char c = spr->data[row * spr->w + px];
            
            // Цвет 0 — прозрачный
            if (c != 0) {
                // Отражаем по Y уже на экране.
                // Если flip_y == 0, то row=0 (верхушка) идет наверх экрана (y + h - 1)
                // Если flip_y == 1, то row=0 идет вниз экрана (y + row)
                int dest_y = y + (flip_y ? row : (spr->h - 1 - row));
                
                naga8_pset(x + col, dest_y, c);
            }
        }
    }
}

void naga8_present(void)
{
    if (!g_inited) return;

    // Обновляем счётчик кадров всегда, чтобы при включении FPS не показывал 0
    naga8_fps_tick();

    // --- Отрисовка FPS (только если включен) ---
    if (g_show_fps) {
        char fps_buf[16];
        snprintf(fps_buf, sizeof(fps_buf), "%d", naga8_fps_get());
        
        // Жестко прибили к правому верхнему углу (X = 112)
        naga8_text(fps_buf, 112, NAGA8_H - 6, 7);
    }
    // ------------------------------------------

    for (int i = 0; i < NAGA8_W * NAGA8_H; i++) {
        Color c = g_pal[g_fb[i] & 15];
        g_rgba[i*4 + 0] = c.r;
        g_rgba[i*4 + 1] = c.g;
        g_rgba[i*4 + 2] = c.b;
        g_rgba[i*4 + 3] = 255;
    }
    UpdateTexture(g_rt.texture, g_rgba);

    BeginDrawing();
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    int scale_x = screenW / NAGA8_W;
    int scale_y = screenH / NAGA8_H;
    int scale   = (scale_x < scale_y) ? scale_x : scale_y;
    if (scale < 1) scale = 1;

    int draw_w  = NAGA8_W * scale;
    int draw_h  = NAGA8_H * scale;
    int offsetX = (screenW - draw_w) / 2;
    int offsetY = (screenH - draw_h) / 2;

    DrawTexturePro(
        g_rt.texture,
        (Rectangle){ 0, 0, NAGA8_W, -NAGA8_H },
        (Rectangle){ (float)offsetX, (float)offsetY,
                     (float)draw_w,  (float)draw_h },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    EndDrawing();
}