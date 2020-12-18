#include "node.hpp"

Node::Node(unsigned board_size) : p_node(nullptr), game(board_size) {
  untry_op = std::vector<int>(game.avai_op);
  c_node_size = game.avai_op.size();
  c_node = new Node*[c_node_size];
  for (int i = 0; i < c_node_size; i++) {
    c_node[i] = nullptr;
  }
  simul_count = 0;
  win_count = 0;
}

Node::Node(Node* node, int move_pos) : p_node(node), game(node->game) {
  game.move(move_pos);
  untry_op = std::vector<int>(game.avai_op);
  c_node_size = game.avai_op.size();
  c_node = new Node*[c_node_size];
  for (int i = 0; i < c_node_size; i++) {
    c_node[i] = nullptr;
  }
  simul_count = 0;
  win_count = 0;
}

Node::~Node() {
  for (int i = 0; i < c_node_size; i++) {
    if (c_node[i] != nullptr) {
      delete c_node[i];
    }
  }
  delete[] c_node;
}

bool Node::tried_all() { return untry_op.size() == 0; }

NodeWithLock::NodeWithLock(unsigned board_size)
    : p_node(nullptr), game(board_size) {
  untry_op = std::vector<int>(game.avai_op);
  c_node_size = game.avai_op.size();
  c_node = new NodeWithLock*[c_node_size];
  for (int i = 0; i < c_node_size; i++) {
    c_node[i] = nullptr;
  }
  simul_count = 0;
  win_count = 0;
}

NodeWithLock::NodeWithLock(NodeWithLock* node, int move_pos)
    : p_node(node), game(node->game) {
  game.move(move_pos);
  untry_op = std::vector<int>(game.avai_op);
  c_node_size = game.avai_op.size();
  c_node = new NodeWithLock*[c_node_size];
  for (int i = 0; i < c_node_size; i++) {
    c_node[i] = nullptr;
  }
  simul_count = 0;
  win_count = 0;
}

NodeWithLock::~NodeWithLock() {
  for (int i = 0; i < c_node_size; i++) {
    if (c_node[i] != nullptr) {
      delete c_node[i];
    }
  }
  delete[] c_node;
}

bool NodeWithLock::tried_all() { return untry_op.size() == 0; }