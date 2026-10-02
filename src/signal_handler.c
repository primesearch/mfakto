/*
This file is part of mfaktc (mfakto).
Copyright (c) 2011-2013  Oliver Weihe (o.weihe@t-online.de)
                         Bertram Franz (bertramf@gmx.net)

mfaktc (mfakto) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

mfaktc (mfakto) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with mfaktc (mfakto).  If not, see <http://www.gnu.org/licenses/>.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !(defined _MSC_VER || defined __MINGW32__)
  #include <unistd.h>
#endif

#include "params.h"
#include "my_types.h"
#include "compatibility.h"

static mystuff_t *signal_handler_mystuff;

/*
On POSIX systems the handler interrupts the main program, which may be in the middle of printf(), malloc() or an
OpenCL call, so it may only use async-signal-safe functions: write() and _exit() instead of printf() and exit().
stdout and the log file are line-buffered (line_buffered()), so _exit() doesn't lose complete lines that were
printed before. On Windows the handler runs in a separate thread, where printf() and exit() are fine.
*/
static void signal_message(const char *msg)
{
#if defined _MSC_VER || defined __MINGW32__
  fputs(msg, stdout);
#else
  if (write(STDOUT_FILENO, msg, strlen(msg)) < 0)
  {
    /* nothing to do */
  }
#endif
}

void my_signal_handler(int signum)
{
#if defined _MSC_VER || defined __MINGW32__
    /* Windows resets the signal handler to the default action once it is invoked so we just register it again. */
    signal(signum, &my_signal_handler);
#else
    (void)signum;
#endif

    signal_handler_mystuff->quit++;
    if (signal_handler_mystuff->quit == 1) {
        signal_message(signal_handler_mystuff->mode == MODE_NORMAL ? "\nmfakto will exit once the current class is finished.\n"
                                                                   : "\nmfakto will exit once the current test is finished.\n");
        signal_message("press ^C again to exit immediately\n");
    }
    else {
        signal_message("mfakto will exit NOW!\n");
#if defined _MSC_VER || defined __MINGW32__
        exit(1);
#else
        _exit(1);
#endif
    }
}

/* make f line-buffered on POSIX systems (see my_signal_handler()); on Windows, _IOLBF means full buffering */
void line_buffered(FILE *f)
{
#if !(defined _MSC_VER || defined __MINGW32__)
  if (f != NULL) setvbuf(f, NULL, _IOLBF, BUFSIZ);
#else
  (void)f;
#endif
}

void register_signal_handler(mystuff_t *mystuff)
{
    signal_handler_mystuff = mystuff;
    signal(SIGINT, &my_signal_handler);
    signal(SIGTERM, &my_signal_handler);
}
