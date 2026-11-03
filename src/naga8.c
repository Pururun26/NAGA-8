#include "naga8_api_py.h"
#include "naga8_conf.h"
#include "naga8_log.h"
#include "naga8_save.h"
#include "pocketpy.h"

#include <stdio.h>
#include <stdlib.h>

#define NAGA8_DEFAULT_CART "cart.n8"

static char* read_file(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    rewind(f);
    char* buf = (char*)malloc((size_t)size + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)size, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char** argv)
{
    naga8_log_init();

    const char* cart = (argc > 1) ? argv[1] : NAGA8_DEFAULT_CART;
    naga8_log("Starting NAGA-8");
    naga8_log("Cartridge: %s", cart);

    py_initialize();
    naga8_bind();               /* регистрирует модуль "naga" */
    naga8_save_open(cart);

    char* code = read_file(cart);
    if (!code) {
        naga8_log("[ERROR] Cannot open cart '%s'", cart);
        fprintf(stderr, "NAGA-8: cannot open cart '%s'\n", cart);
        py_finalize();
        naga8_log_close();
        return 1;
    }

    bool ok = py_exec(code, cart, EXEC_MODE, NULL);
    free(code);

    if (!ok) {
        naga8_log("[ERROR] Python exception during py_exec");
        py_printexc();
    }
    fflush(stdout);
    fflush(stderr);

    py_finalize();
    naga8_save_close();
    naga8_log("Shutting down");
    naga8_log_close();
    return ok ? 0 : 1;
}