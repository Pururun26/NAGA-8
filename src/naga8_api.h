#pragma once

void naga8_poll(void);

void naga8_init(void);
void naga8_close(void);
void naga8_cls(int color);
void naga8_rect(int x, int y, int w, int h, int color);
void naga8_present(void);