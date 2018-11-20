#pragma once

#include <array>
#include <cmath>
#include <vector>
#include <string.h>
#include <iostream>

#define ALPHA_BETA_ENABLE 1

typedef std::array<std::array<char, 8>, 8> Board;
typedef char coor;

enum symbols {
    b = ' ',
    w = '#',
    bm = '+',
    bk = '*',
    wm = '-',
    wk = '%'
};

struct action
{
    Board board;
    char jumps;
    coor path[15];
};


inline bool is_black_player(char c) {
    return c == symbols::bm || c == symbols::bk;
}

inline bool is_white_player(char c) {
    return c == symbols::wm || c == symbols::wk;
}

inline bool is_king(char c) {
    return c == symbols::wk || c == symbols::bk;
}

inline bool is_free(char c) {
    return c == symbols::b || c == symbols::w;
}

inline bool opposite(char c1, char c2) {
    return (is_black_player(c1) && is_white_player(c2)) || 
            (is_black_player(c2) && is_white_player(c1));
}

inline bool valid_coord(int col, int row) {
    return (row >= 0 && row < 8) && (col >= 0 && col < 8);
}

inline char color_square(int x, int y) {
    return ((x + y) & 1)? '#' : ' ';
}

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}


// functions
std::vector<char> next_move(const Board &board, char player, int depth);
void move_to_coords(char move, int &x, int &y);
std::vector<std::vector<char> > get_legal_movements(const Board &board, int x, int y, bool &another_eat, std::vector<Board> *boards = NULL);


