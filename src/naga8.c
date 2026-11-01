#include "naga8_api_py.h"
#include "naga8_conf.h"
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
    const char* cart = (argc > 1) ? argv[1] : NAGA8_DEFAULT_CART;

    py_initialize();
    naga8_bind();               /* регистрирует модуль "naga" */

    char* code = read_file(cart);
    if (!code) {
        fprintf(stderr, "NAGA-8: cannot open cart '%s'\n", cart);
        py_finalize();
        return 1;
    }

    bool ok = py_exec(code, cart, EXEC_MODE, NULL);
    free(code);

    if (!ok) py_printexc();
    fflush(stdout);
    fflush(stderr);

    py_finalize();
    return ok ? 0 : 1;
}