#pragma once

/* Открыть лог рядом с naga8. Вызывать один раз в начале main,
 * ДО InitWindow() — чтобы перехватить стартовые логи raylib. */
void naga8_log_init(void);

/* Закрыть лог. Вызывать перед выходом. */
void naga8_log_close(void);

/* Свои логи (аналог printf, но в файл). */
void naga8_log(const char *format, ...);