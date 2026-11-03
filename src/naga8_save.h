#pragma once
#include <stdint.h>

/* 64 слота × 8 байт = 512 байт.
 * Файл: <application_dir>/save/<cartname>.bin */

void    naga8_save_open(const char* cart_path);
void    naga8_save_close(void);

void    naga8_dset(int slot, int64_t value);
int64_t naga8_dget(int slot);