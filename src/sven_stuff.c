/* sven_stuff.c

Copyright (C) 2024 Steven Marion.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

// wow, this style is... really something. why does options.h not include Imlib2.h? Then again, even Praveen thought that was not good code style, so maybe that was the standard practice at one point...
// nevermind. they include headers from some headers and not from others. This could be cleaned up a lot.

#include <Imlib2.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include "feh.h"
#include "options.h"
#include "filelist.h"
#include "menu.h"
#include "structs.h"
#include "winwidget.h"
#include "sven_stuff.h"

#define LIKELY(a) __builtin_expect((bool)(a), true)
#define UNLIKELY(a) __builtin_expect((bool)(a), false)
#define SVEN_WARN_ERRNO(msg, ...) { \
    const char* error_message = strerror(errno); \
    weprintf(msg ": %s", ##__VA_ARGS__, error_message); \
}
#define SVEN_FATAL_ERRNO(msg, ...) { \
    const char* error_message = strerror(errno); \
    eprintf(msg ": %s", ##__VA_ARGS__, error_message); \
}

#define SVEN_MENU_OPTION_BUFFER_SIZE 40

#define CB_PRINT_TO_SVEN_LOG -1

static FILE* sven_log_out;
static char sven_menu_option[SVEN_MENU_OPTION_BUFFER_SIZE];

static void sven_close_log();

void init_sven_stuff() {
    if (opt.sven_log_path != NULL) {
        int fd_temp = open(opt.sven_log_path, O_WRONLY | O_CREAT | O_EXCL, 0777);
        // doing it this way so it'll fail if the file already exists. There's other ways this could be done, but I like this one
        if (UNLIKELY(fd_temp == -1)) {
            SVEN_FATAL_ERRNO("open: %s", opt.sven_log_path);
        }
        sven_log_out = fdopen(fd_temp, "at");
        if (UNLIKELY(sven_log_out==NULL)) {
            SVEN_FATAL_ERRNO("Could not open %s", opt.sven_log_path);
        }
        snprintf(sven_menu_option, SVEN_MENU_OPTION_BUFFER_SIZE, "Log to %s", opt.sven_log_path);
        atexit(sven_close_log);
    }
}

void sven_add_to_main_menu(feh_menu *m) {
    if (opt.sven_log_path != NULL) {
        feh_menu_add_entry(m, sven_menu_option, NULL, CB_PRINT_TO_SVEN_LOG, 0, NULL);
    }
}

void sven_handle_main_menu_action(feh_menu *m, feh_menu_item *i, int action, unsigned short data) {
    const char* path = FEH_FILE(m->fehwin->file->data)->filename;
    fprintf(sven_log_out, "%s\n", path);
}

static void sven_close_log() {
    if (LIKELY(sven_close_log!=NULL)) {
        int res = fclose(sven_log_out);
        if (UNLIKELY(res!=0))
            SVEN_WARN_ERRNO("fclose: %s", opt.sven_log_path);
    }
}

