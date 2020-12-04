#pragma once
#include <cstdint>
#include <vector>

class Game {
 public:
  unsigned size;
  int prev_move;
  std::vector<int8_t> board;  // -1 no pieces 0,1 player pieces
  std::vector<int> avai_op;   // available position on board
  int8_t player = 0;          // 0 or 1, 0 first move
  int8_t status = 0;          // 0 not finish 1 p0 win 2 tie 3 p1 win
  Game(unsigned _size = 7);   // new game
  Game(const Game& game);     // copy constructer
  ~Game();
  void move(int pos);   // put pieces on board, renew status avai_op player
  bool bingo(int pos);  // don't know what to name this
};