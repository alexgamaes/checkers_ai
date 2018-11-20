#include "engine.h"

using namespace std;

void move_to_coords(char move, int &row, int &col) {
    row = move >> 4;
    col = move & 0x0f;
}

char coords_to_move(int row, int col) {
    return (row << 4) | col;
}

bool can_jump(const Board &board, int row, int col, int rowdirection, int coldirection) {
        return valid_coord(row + rowdirection, col + coldirection) &&
                valid_coord(row + 2 * rowdirection, col + 2 * coldirection) &&
                opposite(board[row][col], board[row + rowdirection][col + coldirection]) &&
                is_free(board[row + 2 * rowdirection][col + 2 * coldirection]);
    
}

bool can_move(const Board &board, int row, int col, int rowdirection, int coldirection) {
        //cout << row + rowdirection << "," <<  col + coldirection << endl;
        return valid_coord(row + rowdirection, col + coldirection) &&
                is_free(board[row + rowdirection][col + coldirection]);
    
} 

bool has_to_eat(const Board &board, int row, int col) {
    if(is_free(board[row][col])) {
        return false;
    } else {
        // White goes up, black down.
        int direction = is_white_player(board[row][col])? -1 : 1;
        
        if(is_king(board[row][col])) {
            // Check NE, NO, SE, SO
            for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
                for(int rowdirection = -1; rowdirection <= 1; rowdirection += 2) {
                    // King can move any square before jump
                    for(int offset = 1; valid_coord(row + offset * rowdirection, col + offset * coldirection); offset++) {
                        // Has to jump
                        if(can_jump(board, row + offset * rowdirection, col + offset * coldirection, rowdirection, coldirection)) {
                            return true;
                        }
                    }
                }
            }
        } else {
            // Only can move forwards, an check if must eat
            if(can_jump(board, row, col, direction, -1) || can_jump(board, row, col, direction, 1)) {
                return true;
            }
            
        }
        
    }
    
    return false;
}

void make_movement_board(const Board &board, int row1, int col1, int row2, int col2, Board &ans) {
    memcpy(ans, board, sizeof(Board));
    
    int top = min(row1, row2);
    int bottom = max(row1, row2);
    int left = min(col1, col2);
    int right = max(col1, col2);
    
    char piece = is_white_player(board[row1][col1]);
    
    for(int c = left + 1, r = top + 1; c < right; r++, c++) {
        if(opposite(board[r][c], piece)) {
            ans[r][c] = color_square(r, c);
            break;
        }
    }
    
    ans[row2][col2] = ans[row1][col1];
    ans[row1][col1] = color_square(row1, col1);
} 

// s: stack of current movements
void get_jumps(vector<char> &s, vector<vector<char> > &movements, const Board &board, int previousrow, int previouscol, int row, int col) {
    // White goes up, black down.
    int direction = is_white_player(board[previousrow][previouscol])? -1 : 1;

    s.push_back(coords_to_move(row, col));    
    movements.push_back(s);
    
    if(is_king(board[previousrow][previouscol])) {
        // Check NE, NO, SE, SO
        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            for(int rowdirection = -1; rowdirection <= 1; rowdirection += 2) {
                // King can move any square before jump
                for(int offset = 1; valid_coord(row + offset * rowdirection, col + offset * coldirection); offset++) {
                    // Has to jump
                    if(can_jump(board, row + offset * rowdirection, col + offset * coldirection, rowdirection, coldirection)) {
                        Board next_board;
                        make_movement_board(board, row, col, row + (offset + 2) * rowdirection, col + (offset + 2) * coldirection, next_board);
                        get_jumps(s, movements, next_board, row, col, row + (offset + 2) * rowdirection, col + (offset + 2) * coldirection);
                    }
                }
            }
        }
    } else {
        // Only can move forwards, an check if must eat
        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            // Has to jump
            if(can_jump(board, row, col, direction, coldirection)) {
                Board next_board;
                make_movement_board(board, row, col, row + (2) * direction, col + (2) * coldirection, next_board);
                get_jumps(s, movements, next_board, row, col, row + (2) * direction, col + (2) * coldirection);
            }
        }
    }
    
    s.pop_back();
}

std::vector<std::vector<char> > get_legal_movements(const Board &board, int row, int col, bool &another_eat) {
    std::vector<std::vector<char> > movements;
    
    bool is_white = is_white_player(board[row][col]);
    
    if(another_eat) {
        if(!has_to_eat(board, row, col)) {
            return movements; // EMPTY
        } 
    } else {

        bool my_piece_eat = false;
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                if(is_white && is_white_player(board[i][j])) {
                    if(has_to_eat(board, i, j)) {
                        another_eat = true;
                        if(row == i && col == j) {
                            my_piece_eat = true;
                        }
                    }
                } else if(!is_white && is_black_player(board[i][j])) {
                    if(has_to_eat(board, i, j)) {
                        another_eat = true;
                        if(row == i && col == j) {
                            my_piece_eat = true;
                        }
                    }
                }
            }
        }
        
        if(another_eat && !my_piece_eat) {
            return movements; // EMPTY
        }
    }
    
    // White goes up, black down.
    int direction = is_white_player(board[row][col])? -1 : 1;
    
    // CALCULATE MOVEMENTS
    if(is_king(board[row][col])) {
        
    } else {
        //cout << row << "*" << col << board[row][col] << endl; 
        // Only can move forwards, an check if must eat
        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            // Has to jump
            if(can_jump(board, row, col, direction, coldirection)) {
                vector<char> s;
                Board next_board;
                make_movement_board(board, row, col, row + (2) * direction, col + (2) * coldirection, next_board);
                get_jumps(s, movements, next_board, row, col, row + (2) * direction, col + (2) * coldirection);
            } else if(can_move(board, row, col, direction, coldirection)) {
                vector<char> s;
                //cout << row + direction << "*" << col + coldirection << endl; 
                s.push_back(coords_to_move(row + direction, col + coldirection));
                movements.push_back(s);
            }
        }
    }
    
    return movements;
}

action next_move(Board board) {
    action a;
    
    return a;
}




