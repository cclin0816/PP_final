#include "mcts.hpp"

#include <algorithm>

MCTS::MCTS(unsigned _size) {
  root = new Node(_size);
  std::random_device rd;
  rg.seed(rd());
}

MCTS::~MCTS() { delete root; }

void MCTS::uct_search(int round) {
  for (int i = 0; i < round; i++) {
    Node* selected = expand(select());
    backprop(simulate(selected), selected);
  }
}

Node* MCTS::select() {
  Node* node_itr = root;
  while (node_itr->tried_all() && node_itr->c_node.size() != 0) {
    std::vector<Node*>& c_node = node_itr->c_node;
    double C_N = EXPLORE_PARAM * log((double)(node_itr->simul_count));
    double max_val = 0;
    int max_pos = 0;
    for (int i = 0; i < c_node.size(); i++) {
      double W = (double)(c_node[i]->win_count);
      double Ni = (double)(c_node[i]->simul_count);
      double ucb_val;

      if (c_node[i]->state.player != 0) {
        ucb_val = 1.0 - W / Ni + sqrt(C_N / Ni);
      } else {
        ucb_val = W / Ni + sqrt(C_N / Ni);
      }
      if (ucb_val > max_val) {
        max_val = ucb_val;
        max_pos = i;
      }
    }
    node_itr = c_node[max_pos];
  }
  return node_itr;
}

Node* MCTS::expand(Node* node) {
  if (node->untry_op.size() != 0) {
    std::uniform_int_distribution<int> rand_choice(0,
                                                   node->untry_op.size() - 1);
    int choice = rand_choice(rg);
    int move_choice = node->untry_op[choice];
    Node* new_node = new Node(node, move_choice);
    node->untry_op.erase(node->untry_op.begin() + choice);
    node->c_node.push_back(new_node);
    node = new_node;
  }
  return node;
}

double MCTS::simulate(Node* node) {
  Game state(node->state);
  while (state.status == 0) {
    std::uniform_int_distribution<int> rand_choice(0, state.avai_op.size() - 1);
    state.move(state.avai_op[rand_choice(rg)]);
  }
  return (state.status - 1) / 2.0;
}

void MCTS::backprop(double result, Node* node) {
  while (node != nullptr) {
    node->simul_count++;
    node->win_count += result;
    node = node->p_node;
  }
}

void MCTS::update_tree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  Node* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new Node(root, move);
  } else {
    int move_node = 0;
    std::vector<Node*>& c_node = root->c_node;
    for (int i = 0; i < c_node.size(); i++) {
      if (c_node[i]->state.prev_move == move) {
        move_node = i;
        break;
      }
    }
    new_node = c_node[move_node];
    c_node.erase(c_node.begin() + move_node);
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS::get_move() {
  double max_val = 0;
  Node* next_node = root->c_node[0];
  for (auto& itr : root->c_node) {
    double score = (double)(itr->win_count) / (double)(itr->simul_count);
    if (itr->state.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next_node = itr;
    }
  }
  return next_node->state.prev_move;
}