#pragma once
#include <cmath>
#include <cstdint>
#include <random>

#include "node.hpp"

#define EXPLORE_PARAM 1.414213562373095

class MCTS {
 public:
  Node* root;
  int8_t player;
  std::mt19937 rg;           // random generator
  MCTS(unsigned _size = 7);  // new search tree
  ~MCTS();
  Node* select();
  Node* expand(Node* node);
  double simulate(Node* node);
  void backprop(double result, Node* node);
  void uct_search(int round);  // run search multiple times
  void update_tree(int move);
  int get_move();
};