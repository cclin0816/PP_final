#include "node.hpp"

Node::Node(unsigned _size) : p_node(nullptr), state(_size) {
  untry_op = std::vector<int>(state.avai_op);
}

Node::Node(Node* node, int move_pos) : p_node(node), state(node->state) {
  state.move(move_pos);
  untry_op = std::vector<int>(state.avai_op);
}

Node::~Node() {
  for(auto &itr : c_node) {
    delete itr;
  }
}

bool Node::tried_all() { return untry_op.size() == 0; }