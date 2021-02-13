#include <stdlib.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "define.hpp"
#include "mcts.hpp"

// using namespace std;

void get_simcount_ref(const char *target, std::vector<int> &simcount_ref,
                      int boardsize) {
  std::string name = target;
  name += ".conf";
  std::fstream ref(name.c_str(), std::fstream::in);
  for (int i = 0; i < boardsize * boardsize; i++) {
    ref >> simcount_ref[i];
  }
  ref.close();
}

int inp_to_move(std::string inp) {
  return (((int) inp[0]) - 65) * 19 + (stoi(inp.substr(1)) - 1);
}

int main() {
  int boardsize = 19;
  std::vector<int> simcount_ref(boardsize * boardsize, 0);
  get_simcount_ref("MCTS_T_L_THD", simcount_ref, boardsize);
  MCTS_T_L_THD agent(19, 4);
  Game game(boardsize);
  int move_count = 0;
  std::string inp;
  int next_move;
  while (game.status == 0) {

    agent.UCTSearch(simcount_ref[move_count]);
    std::cin >> inp;
    while (inp == "print") {
      for (int i = 0; i < boardsize * boardsize; i++) {
        if(game.board[i] != -1)
          std::cout << (int)game.board[i];
        if(i % 19 == 18)
          std::cout << '\n';
        else
          std::cout << '\t';
      }
      std::cin >> inp;
    }
    next_move = inp_to_move(inp);
    agent.UpdateTree(next_move);
    game.move(next_move);
    std::cout << next_move << std::endl;
    // std::cout << "bingo " << game.bingo(next_move) << std::endl;
    move_count++;

    if(game.status != 0) 
      break;

    agent.UCTSearch(simcount_ref[move_count]);
    next_move = agent.GetMove();
    agent.UpdateTree(next_move);
    game.move(next_move);
    move_count++;
    std::cout << next_move << std::endl;
    // std::cout << "bingo " << game.bingo(next_move) << std::endl;
    std::cout << ((char)((next_move / 19) + 65)) << next_move % 19 + 1 << std::endl;
  }
  std::cout << "fin" << (int)game.status << std::endl;
}