#pragma once
#include <atomic>
#include <mutex>
#include <vector>

#include "define.hpp"
#include "game.hpp"

class Node {
 public:
  Game game;                               // game state present by this node
  Node* p_node;                            // parent node
  Node** c_node;                           // child nodes list
  int c_node_size;                         // child node size
  std::vector<int> untry_op;               // untried option
  unsigned simul_count;                    // node simulation coount
  double win_count;                        // node win count
  Node(unsigned board_size = BOARD_SIZE);  // new node used by creating root
  Node(Node* node, int move_pos);  // new node with father then move accordingly
  ~Node();
  bool tried_all();
};

class NodeWithLock {
 public:
  Game game;                          // game state present by this node
  NodeWithLock* p_node;               // parent node
  NodeWithLock** c_node;              // child nodes list
  int c_node_size;                    // child node size
  std::vector<int> untry_op;          // untried option
  std::atomic<unsigned> simul_count;  // node simulation coount
  std::atomic<double> win_count;      // node win count
  std::mutex lck;                     // lock for expansion
  NodeWithLock(
      unsigned board_size = BOARD_SIZE);  // new node used by creating root
  NodeWithLock(NodeWithLock* node,
               int move_pos);  // new node with father then move accordingly
  ~NodeWithLock();
  bool tried_all();
};