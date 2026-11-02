#include "naga8_api_py.h"
#include "naga8_api.h"
#include "pocketpy.h"
#include "raylib.h"

#include <string.h>

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

static bool py_cls(int argc, py_Ref argv)
{
    if (argc < 1) return false;
    naga8_cls((int)py_toint(py_arg(0)));
    py_newnone(py_retval());
    return true;
}

static bool py_pset(int argc, py_Ref argv)
{
    if (argc < 3) return false;
    naga8_pset(
        (int)py_toint(py_arg(0)),
        (int)py_toint(py_arg(1)),
        (int)py_toint(py_arg(2))
    );
    py_newnone(py_retval());
    return true;
}

static bool py_pget(int argc, py_Ref argv)
{
    if (argc < 2) return false;
    int c = naga8_pget(
        (int)py_toint(py_arg(0)),
        (int)py_toint(py_arg(1))
    );
    py_newint(py_retval(), c);
    return true;
}

static bool py_text(int argc, py_Ref argv)
{
    if (argc < 4) return false;
    const char* s = py_tostr(py_arg(0));
    naga8_text(
        s,
        (int)py_toint(py_arg(1)),
        (int)py_toint(py_arg(2)),
        (int)py_toint(py_arg(3))
    );
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

static bool py_btn(int argc, py_Ref argv)
{
    if (argc < 1) return false;
    int i = (int)py_toint(py_arg(0));
    int p = (argc >= 2) ? (int)py_toint(py_arg(1)) : 0; // если p не передан, берем 0
    py_newbool(py_retval(), naga8_btn(i, p));
    return true;
}

static bool py_btnp(int argc, py_Ref argv)
{
    if (argc < 1) return false;
    int i = (int)py_toint(py_arg(0));
    int p = (argc >= 2) ? (int)py_toint(py_arg(1)) : 0;
    py_newbool(py_retval(), naga8_btnp(i, p));
    return true;
}

static bool py_sprdata(int argc, py_Ref argv)
{
    if (argc < 2) return false;
    int id = (int)py_toint(py_arg(0));
    py_Ref arg = py_arg(1);

    // Если передали не кортеж — просто игнорируем
    if (!py_istuple(arg)) {
        py_newnone(py_retval());
        return true;
    }

    int n = py_tuple_len(arg);
    int total = 0;
    
    // Считаем общий размер (длина строк + символы '\n')
    for (int i = 0; i < n; i++) {
        total += (int)strlen(py_tostr(py_tuple_getitem(arg, i))) + 1;
    }

    char* buf = (char*)malloc(total + 1);
    if (!buf) return false;

    // Склеиваем строки из кортежа через '\n'
    int pos = 0;
    for (int i = 0; i < n; i++) {
        const char* s = py_tostr(py_tuple_getitem(arg, i));
        int slen = (int)strlen(s);
        memcpy(buf + pos, s, slen);
        pos += slen;
        if (i < n - 1) buf[pos++] = '\n';
    }
    buf[pos] = '\0';

    // Отправляем в C-парсер
    naga8_sprdata(id, buf);
    free(buf);

    py_newnone(py_retval());
    return true;
}

static bool py_spr(int argc, py_Ref argv)
{
    if (argc < 3) return false;
    int id = (int)py_toint(py_arg(0));
    int x = (int)py_toint(py_arg(1));
    int y = (int)py_toint(py_arg(2));
    int fx = (argc >= 4) ? (int)py_toint(py_arg(3)) : 0;
    int fy = (argc >= 5) ? (int)py_toint(py_arg(4)) : 0;
    
    naga8_spr(id, x, y, fx, fy);
    py_newnone(py_retval());
    return true;
}

void naga8_bind(void)
{
    py_GlobalRef mod = py_newmodule("naga");
    py_bind(mod, "run(update, draw)",       py_run);
    py_bind(mod, "cls(color)",              py_cls);
    py_bind(mod, "pset(x, y, color)",       py_pset);
    py_bind(mod, "pget(x, y)",              py_pget);
    py_bind(mod, "text(s, x, y, color)",    py_text);
    py_bind(mod, "rect(x, y, w, h, color)", py_rect);
    py_bind(mod, "btn(i, p=0)",             py_btn);
    py_bind(mod, "btnp(i, p=0)",            py_btnp);
    py_bind(mod, "sprdata(id, data)",       py_sprdata);
    py_bind(mod, "spr(id, x, y, flip_x=0, flip_y=0)", py_spr);
}