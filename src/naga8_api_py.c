#include "naga8_api_py.h"
#include "naga8_api.h"
#include "pocketpy.h"
#include "raylib.h"

static bool py_cls(int argc, py_Ref argv)
{
    if (argc < 1) return false;
    naga8_cls((int)py_toint(py_arg(0)));
    py_newnone(py_retval());
    return true;
}

static bool py_rect(int argc, py_Ref argv)
{
    if (argc < 5) return false;
    naga8_rect(
        (int)py_toint(py_arg(0)),
        (int)py_toint(py_arg(1)),
        (int)py_toint(py_arg(2)),
        (int)py_toint(py_arg(3)),
        (int)py_toint(py_arg(4))
    );
    py_newnone(py_retval());
    return true;
}

static bool py_run(int argc, py_Ref argv)
{
    if (argc != 2) return false;

    naga8_init();
    while (!WindowShouldClose()) {
        naga8_poll();
        
        if (!py_call(py_arg(0), 0, NULL)) { py_printexc(); break; }
        if (!py_call(py_arg(1), 0, NULL)) { py_printexc(); break; }
        naga8_present();
    }
    naga8_close();
    py_newnone(py_retval());
    return true;
}

void naga8_bind(void)
{
    py_GlobalRef mod = py_newmodule("naga");
    py_bind(mod, "cls(color)",              py_cls);
    py_bind(mod, "rect(x, y, w, h, color)", py_rect);
    py_bind(mod, "run(update, draw)",       py_run);
}