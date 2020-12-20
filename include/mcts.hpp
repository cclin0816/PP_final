#pragma once
#include <cmath>
#include <cstdint>
#include <random>
#include <thread>

#include "define.hpp"
#include "node.hpp"

class M {
 public:
  virtual void UCTSearch(int round) {}
  virtual void UpdateTree(int move) {}
  virtual int GetMove() { return 0; }
};

class MCTS : public M {
 public:
  Node* root;
  std::mt19937 rg;                 // random generator
  MCTS(unsigned board_size = 13);  // new search tree
  ~MCTS();
  void UCTSearch(int round);  // run search multiple times
  void UpdateTree(int move);  // update tree with move
  int GetMove();              // get current best move
};

class MCTS_L_THD : public M {
 public:
  Node* root;
  std::mt19937 rg;  // random generator
  unsigned thdn;
  std::vector<std::thread> thds;
  std::vector<std::mt19937> rgs;
  std::vector<double> results;
  int load;
  MCTS_L_THD(unsigned board_size = 13,
             unsigned thread_num = 1);  // new search tree
  ~MCTS_L_THD();
  void UCTSearch(int round);  // run search multiple times
  void UpdateTree(int move);  // update tree with move
  int GetMove();              // get current best move
};

class MCTS_R_THD : public M {
 public:
  std::vector<Node*> roots;
  // std::mt19937 rg;  // random generator
  unsigned thdn;
  std::vector<std::thread> thds;
  std::vector<std::mt19937> rgs;
  MCTS_R_THD(unsigned board_size = 13,
             unsigned thread_num = 1);  // new search tree
  ~MCTS_R_THD();
  void UCTSearch(int round);  // run search multiple times
  void UpdateTree(int move);  // update tree with move
  int GetMove();              // get current best move
};

class MCTS_T_G_THD : public M {
 public:
  Node* root;
  unsigned thdn;
  std::vector<std::thread> thds;
  std::vector<std::mt19937> rgs;
  std::mutex lck_exp, lck_bkp;
  MCTS_T_G_THD(unsigned board_size = 13, unsigned thread_num = 1);
  ~MCTS_T_G_THD();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_T_L_THD : public M {
 public:
  NodeWithLock* root;
  unsigned thdn;
  std::vector<std::thread> thds;
  std::vector<std::mt19937> rgs;
  MCTS_T_L_THD(unsigned board_size = 13, unsigned thread_num = 1);
  ~MCTS_T_L_THD();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

