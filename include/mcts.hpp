#pragma once
#include <cmath>
#include <cstdint>
#include <random>
#include <thread>

#include "define.hpp"
#include "node.hpp"

class M {
 public:
  virtual void UCTSearch(int round) {}  // search tree with * round
  virtual void UpdateTree(int move) {}  // update tree with move
  virtual int GetMove() { return 0; }   // get best move
};

class MCTS : public M {
 public:
  Node* root;                              // root node
  std::mt19937 rg;                         // random generator
  MCTS(unsigned board_size = BOARD_SIZE);  // new search tree
  ~MCTS();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_L_THD : public M {
 public:
  Node* root;                     // root node
  std::mt19937 rg;                // random generator
  unsigned thdn;                  //  number of thread
  std::vector<std::thread> thds;  // threads
  std::vector<std::mt19937> rgs;  // random generator for thread
  std::vector<double> results;    // results for thread
  int load;  // load if for leaf parallization that runs simulate for load times
             // to avoid waiting
  MCTS_L_THD(unsigned board_size = BOARD_SIZE,
             unsigned thread_num = 1);  // new search tree
  ~MCTS_L_THD();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_R_THD : public M {
 public:
  std::vector<Node*> roots;       // root node for each thread
  unsigned thdn;                  // number of thread
  std::vector<std::thread> thds;  // threads
  std::vector<std::mt19937> rgs;  // random generator for thread
  MCTS_R_THD(unsigned board_size = BOARD_SIZE,
             unsigned thread_num = 1);  // new search tree
  ~MCTS_R_THD();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_T_G_THD : public M {
 public:
  Node* root;                     // root node
  unsigned thdn;                  //  number of thread
  std::vector<std::thread> thds;  // threads
  std::vector<std::mt19937> rgs;  // random generator for thread
  std::mutex lck_exp, lck_bkp;    // lock for expansion and backpropagation
  MCTS_T_G_THD(unsigned board_size = BOARD_SIZE,
               unsigned thread_num = 1);  // new search tree
  ~MCTS_T_G_THD();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_T_L_THD : public M {
 public:
  NodeWithLock* root;             // root node
  unsigned thdn;                  //  number of thread
  std::vector<std::thread> thds;  // threads
  std::vector<std::mt19937> rgs;  // random generator for thread
  MCTS_T_L_THD(unsigned board_size = BOARD_SIZE,
               unsigned thread_num = 1);  // new search tree
  ~MCTS_T_L_THD();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_L_OMP : public M {
 public:
  Node* root;                     // root node
  std::mt19937 rg;                // random generator
  unsigned thdn;                  //  number of thread
  std::vector<std::mt19937> rgs;  // random generator for thread
  int load;  // load if for leaf parallization that runs simulate for load times
             // to avoid waiting
  MCTS_L_OMP(unsigned board_size = BOARD_SIZE,
             unsigned thread_num = 1);  // new search tree
  ~MCTS_L_OMP();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_R_OMP : public M {
 public:
  std::vector<Node*> roots;       // root node for each thread
  unsigned thdn;                  // number of thread
  std::vector<std::mt19937> rgs;  // random generator for thread
  MCTS_R_OMP(unsigned board_size = BOARD_SIZE,
             unsigned thread_num = 1);  // new search tree
  ~MCTS_R_OMP();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};

class MCTS_T_L_OMP : public M {
 public:
  NodeWithLock* root;             // root node
  unsigned thdn;                  //  number of thread
  std::vector<std::mt19937> rgs;  // random generator for thread
  MCTS_T_L_OMP(unsigned board_size = BOARD_SIZE,
               unsigned thread_num = 1);  // new search tree
  ~MCTS_T_L_OMP();
  void UCTSearch(int round);
  void UpdateTree(int move);
  int GetMove();
};