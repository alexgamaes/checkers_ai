#pragma once

#include "engine.h"

struct CBmove
{
    int jumps; // number of jumps in this move
    int newpiece; // moving piece after jump
    int oldpiece; // moving piece before jump
    struct coordinate from, to; // from,to squares of moving piece
    struct coordinate path[12];	// intermediate squares to jump to
    struct coordinate del[12];	// squares where men are removed
    int delpiece[12]; // piece type which is removed
};

struct coordinate
{
    int x;
    int y;
};