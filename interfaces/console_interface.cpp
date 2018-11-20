#include "engine.h"
#include <stdio.h>

#include <iostream>
#include <string>
#include <assert.h>

int main() {
    /*
    action a = next_move(board);*/
    Board board;

    std::string command;
    std::cin >> command;
    
    //std::cout << command << " = ";
    
    if (command == "get_next_movement") {
    
    } else if (command == "get_legal_movements") {
        int x, y;
        
        std::cin >> x >> y;
        
        //std::cout << x << "|" << y << ":";
        
        std::string aux;
        getline(std::cin, aux);
        for (int i = 0; i < 8; i++) {
            std::string row;
            
            getline(std::cin, row);
            //std::cin >> row;
            for(int j = 0; j < 8; j++) {
                board[i][j] = row[j];
            }
            
            
            //std::cout << row << ";";
        }
        
        bool another_eat = false;
        std::vector<std::vector<char> > movements = get_legal_movements(board, x, y, another_eat);
        
        std::cout << movements.size() << "|";
        
        for(int i = 0; i < movements.size(); i++) {
            for(int j = 0; j < movements[i].size(); j++) {
                int x1, y1;
                move_to_coords(movements[i][j], x1, y1);
                std::cout << x1 << " " << y1 << " ";
            }
            
            std::cout << "|";
            
        }
    }
    
    
    
    return 0;
}
