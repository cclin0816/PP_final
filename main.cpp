#include <iostream>

#include "mcts.hpp"

int main() {
  std::ios_base::sync_with_stdio(false);
  int sum = 0;
  for (int i = 0; i < 500; i++) {
    MCTS p1(13);
    MCTS_L_THD p2(13, 4);
    Game game(13);
    while (true) {
      p1.UCTSearch(2048);
      int move = p1.GetMove();
      game.move(move);
      if (game.status != 0) break;
      p1.UpdateTree(move);
      p2.UpdateTree(move);
      p2.UCTSearch(2048);
      move = p2.GetMove();
      game.move(move);
      if (game.status != 0) break;
      p1.UpdateTree(move);
      p2.UpdateTree(move);
    }
    sum += (int)game.status;
    if (i % 10 == 0) std::cout << (sum / (double)(i + 1) - 1) / 2 << std::endl;
  }
  std::cout << (sum / 500.0 - 1) / 2 << std::endl;
}