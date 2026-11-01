#include "naga8_api.h"
#include "naga8_conf.h"
#include "raylib.h"

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

void naga8_poll(void)
{
    if (!g_inited) return;

    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
}

void naga8_init(void)
{
    if (g_inited) return;
    InitWindow(NAGA8_W * NAGA8_SCALE, NAGA8_H * NAGA8_SCALE, "NAGA-8");
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

void naga8_present(void)
{
    if (!g_inited) return;

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