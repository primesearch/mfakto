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
#if defined _MSC_VER || defined __MINGW32__
  #include <intrin.h>
#else
  #include <unistd.h>
#endif

#include "params.h"
#include "my_types.h"
#include "compatibility.h"

static mystuff_t *signal_handler_mystuff;

/*
The first ^C lets mfakto finish the current class, the second one stops the class early; in both cases the main
loop exits normally. A third ^C exits immediately.

On POSIX systems the handler interrupts the main program, which may be in the middle of printf(), malloc() or an
OpenCL call, so it may only use async-signal-safe functions: write() and _exit() instead of printf() and exit().
stdout and the log file are unbuffered (unbuffered()), so _exit() doesn't lose anything that was printed before.
On Windows the handler runs in a separate thread, where printf() and exit() are fine.
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

    long n;

    /* decide by the value of this increment and don't read quit again: on Windows, each ^C runs the handler on a new
       thread, so handlers can overlap, and the increment must be atomic. On POSIX systems the handler can't be
       interrupted by another SIGINT or SIGTERM (see register_signal_handler()). */
#if defined _MSC_VER || defined __MINGW32__
    n = _InterlockedIncrement(&signal_handler_mystuff->quit);
#else
    n = ++signal_handler_mystuff->quit;
#endif
    if (n == 1) {
        signal_message(signal_handler_mystuff->mode == MODE_NORMAL ? "\nmfakto will exit once the current class is finished.\n"
                                                                   : "\nmfakto will exit once the current test is finished.\n");
        signal_message(signal_handler_mystuff->mode == MODE_NORMAL ? "press ^C again to stop the current class and exit\n"
                                                                   : "press ^C again to stop the current test and exit\n");
    }
    else if (n == 2) {
        /* the main loop stops the class early and exits normally, so files are closed and lock files removed */
        signal_message(signal_handler_mystuff->mode == MODE_NORMAL ? "mfakto will stop the current class and exit.\n"
                                                                   : "mfakto will stop the current test and exit.\n");
        signal_message("press ^C again to exit immediately\n");
    }
    else {
        /* last resort, such as when mfakto is waiting for a lock file */
        signal_message("mfakto will exit NOW!\n");
#if defined _MSC_VER || defined __MINGW32__
        exit(1);
#else
        _exit(1);
#endif
    }
}

/* make f unbuffered (see my_signal_handler()) */
void unbuffered(FILE *f)
{
  if (f != NULL) setvbuf(f, NULL, _IONBF, 0);
}

void register_signal_handler(mystuff_t *mystuff)
{
    signal_handler_mystuff = mystuff;
#if defined _MSC_VER || defined __MINGW32__
    signal(SIGINT, &my_signal_handler);
    signal(SIGTERM, &my_signal_handler);
#else
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &my_signal_handler;
    /* block both signals while the handler runs, so that a SIGTERM can't interrupt the handler of a SIGINT (or vice
       versa) between reading and writing quit */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_flags = SA_RESTART; /* restart interrupted system calls, as signal() does on Linux */
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#endif
}
