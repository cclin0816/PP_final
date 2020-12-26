#pragma once
#include <atomic>
#include <mutex>
#include <vector>

#include "game.hpp"
#include "define.hpp"

class Node {
 public:
  Game game;      // game state present by this node
  Node* p_node;   // parent node
  Node** c_node;  // child nodes list
  int c_node_size;
  std::vector<int> untry_op;
  unsigned simul_count;
  double win_count;
  Node(unsigned board_size = BOARD_SIZE);  // new node used by creating root
  Node(Node* node, int move_pos);  // new node with father
  ~Node();
  bool tried_all();
};

class NodeWithLock {
 public:
  Game game;              // game state present by this node
  NodeWithLock* p_node;   // parent node
  NodeWithLock** c_node;  // child nodes list
  int c_node_size;
  std::vector<int> untry_op;
  std::atomic<unsigned> simul_count;
  std::atomic<double> win_count;
  std::mutex lck;
  NodeWithLock(unsigned board_size = BOARD_SIZE);  // new node used by creating root
  NodeWithLock(NodeWithLock* node, int move_pos);  // new node with father
  ~NodeWithLock();
  bool tried_all();
};