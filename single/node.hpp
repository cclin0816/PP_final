#pragma once
#include <vector>

#include "game.hpp"

class Node {
 public:
  Game state;                 // game state present by this node
  Node* p_node;               // parent node
  std::vector<Node*> c_node;  // child nodes list
  std::vector<int> untry_op;
  unsigned simul_count = 0;
  double win_count = 0;
  Node(unsigned _size = 7);        // new node used by creating root
  Node(Node* node, int move_pos);  // new node with father
  ~Node();
  bool tried_all();
};