#include "engine.h"

using namespace std;

void move_to_coords(char move, int &row, int &col) {
    row = move >> 4;
    col = move & 0x0f;
}

char coords_to_move(int row, int col) {
    return (row << 4) | col;
}


void print_board(const Board &board) {
        
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 8; c++) {
            cout << board[r][c];
        }
        cout << endl;
    }
}


int heuristic(const Board &board, char player) {
    int ans = 0;
    
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 8; c++) {
            if(is_white_player(board[r][c])) {
                ans += (is_king(board[r][c]))? 8 : 5;
            } else if(is_black_player(board[r][c])) {
                ans -= (is_king(board[r][c]))? 8 : 5;
            }
        }
    }
    
    //print_board(board);
    //cout << player << " " << ((player == 'w')? ans : -ans) << endl;
    
    return (player == 'w')? ans : -ans;
}

bool upgrade_to_king(Board &board, int r, int c) {
    if (is_king(board[r][c])) {
        return false;
    }
    
    if(is_white_player(board[r][c])) {
        board[r][c] = symbols::wk;
    } else if (is_black_player(board[r][c])) {
        board[r][c] = symbols::bk;
    }
    
    return true;
}

bool should_upgrade(const Board &board, int r, int c) {
    if (is_king(board[r][c])) {
        return false;
    }
    
    if(is_white_player(board[r][c]) && r == 0) {
        return true;
    } else if (is_black_player(board[r][c]) && r == 7) {
        return true;
    }
    
    return false;
}


char check_mate(const Board &board) {
    int whitePieces = 0;
    int blackPieces = 0;
    
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 8; c++) {
            if(is_white_player(board[r][c])) {
                whitePieces += 1;
            } else if(is_black_player(board[r][c])) {
                blackPieces += 1;
            }
        }
    }
    
    if(whitePieces > 0 && blackPieces > 0) {
        return 0;
    } else if(whitePieces == blackPieces) {
        return 't';
    } else if(whitePieces == 0) {
        return 'b';   
    } else if(blackPieces == 0) {
        return 'w';
    }
    
    return 0;
}

bool can_jump(const Board &board, int row, int col, int rowdirection, int coldirection) {
        return valid_coord(row + rowdirection, col + coldirection) &&
                valid_coord(row + sign(rowdirection) + rowdirection, col + sign(coldirection) + coldirection) &&
                opposite(board[row][col], board[row + rowdirection][col + coldirection]) &&
                is_free(board[row + sign(rowdirection) + rowdirection][col + sign(coldirection) + coldirection]);
    
}

bool can_move(const Board &board, int row, int col, int rowdirection, int coldirection) {
        //cout << row + rowdirection << " " << col + coldirection << board[row + rowdirection][col + coldirection] << endl;
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
                        if(can_jump(board, row, col, offset * rowdirection, offset * coldirection)) {
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

Board make_movement_board(const Board &board, int row1, int col1, int row2, int col2) {
    Board ans = board;
    
    char piece = board[row1][col1];
    
    int delta_row = sign(row2 - row1);
    int delta_col = sign(col2 - col1);
    
    for(int r = row1, c = col1; r != row2; r += delta_row, c += delta_col) {
        //cout << "opposite " << r << " " << c << endl;
        if(opposite(board[r][c], piece)) {
            ans[r][c] = color_square(r, c);
            break;
        }
    }
    
    ans[row2][col2] = ans[row1][col1];
    ans[row1][col1] = color_square(row1, col1);
    
    return ans;
} 

// s: stack of current movements
void get_jumps(vector<char> &s, vector<vector<char> > &movements, const Board &board, int previousrow, int previouscol, int row, int col, vector<Board> *boards = NULL) {
    // White goes up, black down.
    int direction = is_white_player(board[row][col])? -1 : 1;

    s.push_back(coords_to_move(row, col));    
    movements.push_back(s);

    //print_board(board);
    //cout << board[row][col] << endl;
    
    if(is_king(board[row][col])) {
        // Check NE, NO, SE, SO

        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            for(int rowdirection = -1; rowdirection <= 1; rowdirection += 2) {
                // King can move any square before jump

                for(int offset = 1; valid_coord(row + offset * rowdirection, col + offset * coldirection); offset++) {
                    // Has to jump
                    if(can_jump(board, row, col, offset * rowdirection, offset * coldirection)) {
                        Board next_board = make_movement_board(board, row, col, row + (offset + 1) * rowdirection, col + (offset + 1) * coldirection);
                        get_jumps(s, movements, next_board, row, col, row + (offset + 1) * rowdirection, col + (offset + 1) * coldirection);
                    }
                }
            }
        }
    } else {

        // Only can move forwards, an check if must eat
        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            // Has to jump
            if(can_jump(board, row, col, direction, coldirection)) {
                Board next_board = make_movement_board(board, row, col, row + (2) * direction, col + (2) * coldirection);
                get_jumps(s, movements, next_board, row, col, row + (2) * direction, col + (2) * coldirection);
            }
        }
    }
    
    if(boards) {
        if(should_upgrade(board, row, col)) {
            Board upgraded_board = board;
            
            upgrade_to_king(upgraded_board, row, col);
            boards->push_back(upgraded_board);
        } else {
            boards->push_back(board);
        }
        
    }
    
    s.pop_back();
}

std::vector<std::vector<char> > get_legal_movements(const Board &board, int row, int col, bool &another_eat, vector<Board> *boards) {
    std::vector<std::vector<char> > movements;
    
    bool is_white = is_white_player(board[row][col]);
    bool my_piece_eat = false;
    
    if(another_eat) {
        if(!has_to_eat(board, row, col)) {
            return movements; // EMPTY
        } 
    } else {

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
    
        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            for(int rowdirection = -1; rowdirection <= 1; rowdirection += 2) {
                for(int offset = 1; valid_coord(row + offset * rowdirection, col + offset * coldirection); offset++) {
                    if(can_jump(board, row, col, offset * rowdirection, offset * coldirection)) {
                        vector<char> s;
                        Board next_board = make_movement_board(board, row, col, row + (1 + offset) * rowdirection, col + (1 + offset) * coldirection);
                        
                        get_jumps(s, movements, next_board, row, col, row + (1 + offset) * rowdirection, col + (1 + offset) * coldirection, boards);
                        break;
                    } else if(!another_eat && can_move(board, row, col, offset * rowdirection, offset * coldirection)) {
                        vector<char> s;
                        s.push_back(coords_to_move(row + offset * rowdirection, col + offset * coldirection));
                        
                        if(boards) {
                            Board next_board = make_movement_board(board, row, col, row + offset * rowdirection, col + offset * coldirection);
                            
                            if(should_upgrade(next_board, row + offset * rowdirection, col + offset * coldirection)) {
                                upgrade_to_king(next_board, row + offset * rowdirection, col + offset * coldirection);
                            }
                            boards->push_back(next_board);
                        }
                        
                        
                        movements.push_back(s);
                    } else if(!my_piece_eat) {
                        break;
                    }
                }
            }
        }
        
    } else {
        //cout << row << "*" << col << board[row][col] << endl; 
        // Only can move forwards, an check if must eat
        for(int coldirection = -1; coldirection <= 1; coldirection += 2) {
            // Has to jump
            if(can_jump(board, row, col, direction, coldirection)) {
                vector<char> s;
                Board next_board = make_movement_board(board, row, col, row + (2) * direction, col + (2) * coldirection);
                get_jumps(s, movements, next_board, row, col, row + (2) * direction, col + (2) * coldirection, boards);
            } else if(!another_eat && can_move(board, row, col, direction, coldirection)) {
                vector<char> s;
                s.push_back(coords_to_move(row + direction, col + coldirection));
                
                if(boards) {
                    Board next_board = make_movement_board(board, row, col, row + direction, col + coldirection);
                    
                    if(should_upgrade(next_board, row + direction, col + coldirection)) {
                        upgrade_to_king(next_board, row + direction, col + coldirection);
                    }
                    boards->push_back(next_board);
                }
                
                
                movements.push_back(s);
            }
        }
    }
    
    return movements;
}

// Initial values of 
// Aplha and Beta 
const int MAX = 10000; 
const int MIN = -10000;

int minimax(const Board &board, bool maximizingPlayer, char player, int depth, int maxDepth, int alpha, int beta, vector<char> &out) {
    char mate = check_mate(board);
    
    int number_of_movements = 0; // Check no movements
    
    if(mate) {
        if(mate == 't') {
            return 0;
        }
        
        return (player == mate)? -1000 : 1000;
    }
    
    if(depth >= maxDepth) {
        return heuristic(board, player);
    }
    
    if(maximizingPlayer) {
        int best = MIN;
        
        bool another_eat = false;
        bool brk = false;
        
        for(int r = 0; r < 8 && !brk; r++) {
            for(int c = 0; c < 8 && !brk; c++) {
                if(player == 'w' && !is_white_player(board[r][c]) || player == 'b' && !is_black_player(board[r][c])) {
                    continue;
                }
                
                vector<Board> boards;
                vector<vector<char> > movements = get_legal_movements(board, r, c, another_eat, &boards);
                
                for(int i = 0; i < boards.size(); i++) {
                    number_of_movements++;
                    
                    int val = minimax(boards[i], false, (player == 'w')? 'b' : 'w', depth + 1, maxDepth, alpha, beta, out);
                    
                    if(val > best) {
                        best = val;
                        if(depth == 0) {
                            //print_board(board);
                            int x, y;
                            move_to_coords(movements[i][0], x, y);
                            movements[i].insert(movements[i].begin(), coords_to_move(r, c));
                            out = movements[i];
                        }
                    }
                    
                    if(best > alpha) {
                        alpha = best;
                    }

#ifdef ALPHA_BETA_ENABLE
                    if(beta <= alpha) {
                        //cout << "TOT" << endl;
                        brk = true;
                        break;
                    }
#endif
                } 
            }
        }
        
        return (number_of_movements > 0)? best : -1000;
    } else {
        int best = MAX;
        
        bool another_eat = false;
        bool brk = false;
        
        for(int r = 0; r < 8 && !brk; r++) {
            for(int c = 0; c < 8 && !brk; c++) {
                if(player == 'w' && !is_white_player(board[r][c]) || player == 'b' && !is_black_player(board[r][c])) {
                    continue;
                }
                
                vector<Board> boards;
                vector<vector<char> > movements = get_legal_movements(board, r, c, another_eat, &boards);
                
                for(int i = 0; i < boards.size(); i++) {
                    number_of_movements++;
                
                    int val = minimax(boards[i], true, (player == 'w')? 'b' : 'w', depth + 1, maxDepth, alpha, beta, out);
                    
                    if(val < best) {
                        best = val;
                    }
                    
                    if(best < beta) {
                        beta = best;
                    }
                    
#ifdef ALPHA_BETA_ENABLE
                    if(beta <= alpha) {
                        //cout << "TOT" << endl;
                        brk = true;
                        break;
                    }
#endif
                } 
            }
        }
        
        return (number_of_movements > 0)? best : -1000;
    }
}

vector<char> next_move(const Board &board, char player, int depth) {
    vector<char> a;
    
    minimax(board, true, player, 0, depth, MIN, MAX, a);
    
    return a;
}




