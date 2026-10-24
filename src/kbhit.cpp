// simulate _kbhit() on Linux
// taken from https://web.archive.org/web/20200224015457/http://linux-sxs.org/programming/kbhit.html

#ifndef _WIN32

#include "kbhit.h"
#include <unistd.h> // read()
#include <iostream>

keyboard::keyboard()
{
    peek_character=-1;
    configured = false;
    configure();
}

keyboard::~keyboard()
{
    if (configured && foreground_terminal()) tcsetattr(0, TCSANOW, &initial_settings);
}

/*
Keys are only read from a terminal while mfakto runs in the foreground. read() would block if stdin is a pipe or a
socket (such as when mfakto is started by another program), and a background process that changes the terminal
settings or reads from the terminal is stopped (SIGTTOU/SIGTTIN).
*/
bool keyboard::foreground_terminal()
{
    return isatty(0) && tcgetpgrp(0) == getpgrp();
}

/* switch the terminal to non-canonical mode, once mfakto runs in the foreground */
void keyboard::configure()
{
    if (configured || !foreground_terminal()) return;
    if (tcgetattr(0,&initial_settings) != 0) return;
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
//    new_settings.c_lflag &= ~ECHO;
//    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    configured = true;
}

int keyboard::kbhit()
{
unsigned char ch;
int nread;

    if (peek_character != -1) return 1;
    configure();
    if (!configured || !foreground_terminal()) return 0;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &new_settings);

    if (nread == 1)
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}

int keyboard::getch()
{
char ch;

    if (peek_character != -1)
    {
        ch = peek_character;
        peek_character = -1;
    }
    else {
        ssize_t res = read(0, &ch, 1);
        if (res == -1) {
            std::cerr << "Error getting character from terminal\n";
        }
    }

    return ch;
}

#endif
