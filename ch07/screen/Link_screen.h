#ifndef LINK_SCREEN_H
#define LINK_SCREEN_H

#include "Screen.h"

struct Link_screen {
    Screen window;
    Link_screen *next;
    Link_screen *prev;
};

#endif
