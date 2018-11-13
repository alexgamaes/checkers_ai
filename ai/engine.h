#pragma once

#include <vector>

typedef char Board[8][8];
typedef char coor;

struct action
{
    Board board;
    char jumps;
    coor path[15];
};

// functions
action next_move(Board board);