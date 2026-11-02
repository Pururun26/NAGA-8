#pragma once

#include <stdbool.h>

void naga8_poll(void);
void naga8_init(void);
void naga8_close(void);

void naga8_cls(int color);
void naga8_pset(int x, int y, int color);
int  naga8_pget(int x, int y);
void naga8_text(const char* s, int x, int y, int color);

void naga8_rect(int x, int y, int w, int h, int color);

bool naga8_btn(int i, int p);
bool naga8_btnp(int i, int p);

void naga8_sprdata(int id, const char* data);
void naga8_spr(int id, int x, int y, int flip_x, int flip_y);

void naga8_present(void);