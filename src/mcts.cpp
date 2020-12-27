#include "mcts.hpp"

#include <algorithm>
#include <iterator>
#include <omp.h>

Node* NodeSelect(Node* root) {
  Node* node_itr = root;
  while (node_itr->tried_all() && node_itr->c_node_size != 0) {
    Node** c_node = node_itr->c_node;
    int c_node_size = node_itr->c_node_size;
    double C_N = EXPLORE_PARAM * log((double)(node_itr->simul_count));
    double max_val = 0;
    int max_pos = 0;
    for (int i = 0; i < c_node_size; i++) {
      double W = (double)(c_node[i]->win_count);
      double Ni = (double)(c_node[i]->simul_count);
      double ucb_val;

      if (c_node[i]->game.player != 0) {
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

NodeWithLock* NodeWithLockSelect(NodeWithLock* root) {
  NodeWithLock* node_itr = root;
  while (node_itr->tried_all() && node_itr->c_node_size != 0) {
    NodeWithLock** c_node = node_itr->c_node;
    int c_node_size = node_itr->c_node_size;
    double C_N = EXPLORE_PARAM * log((double)(node_itr->simul_count));
    double max_val = 0;
    int max_pos = 0;
    for (int i = 0; i < c_node_size; i++) {
      double W = (double)(c_node[i]->win_count);
      double Ni = (double)(c_node[i]->simul_count);
      double ucb_val;

      if (c_node[i]->game.player != 0) {
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

int FindMove(int move, std::vector<int>& avai_op) {
  return std::distance(avai_op.begin(),
                       std::find(avai_op.begin(), avai_op.end(), move));
}

Node* NodeExpand(Node* node, std::mt19937& rg) {
  if (node->untry_op.size() != 0) {
    std::uniform_int_distribution<int> rand_choice(0,
                                                   node->untry_op.size() - 1);
    int choice = rand_choice(rg);
    int move_choice = node->untry_op[choice];
    Node* new_node = new Node(node, move_choice);
    node->c_node[FindMove(move_choice, node->game.avai_op)] = new_node;
    node->untry_op.erase(node->untry_op.begin() + choice);
    node = new_node;
  }
  return node;
}

NodeWithLock* NodeWithLockExpand(NodeWithLock* node, std::mt19937& rg) {
  if (node->untry_op.size() != 0) {
    std::uniform_int_distribution<int> rand_choice(0,
                                                   node->untry_op.size() - 1);
    int choice = rand_choice(rg);
    int move_choice = node->untry_op[choice];
    NodeWithLock* new_node = new NodeWithLock(node, move_choice);
    node->c_node[FindMove(move_choice, node->game.avai_op)] = new_node;
    node->untry_op.erase(node->untry_op.begin() + choice);
    node = new_node;
  }
  return node;
}

double Simulate(const Game& g, std::mt19937& rg) {
  Game game(g);
  while (game.status == 0) {
    std::uniform_int_distribution<int> rand_choice(0, game.avai_op.size() - 1);
    game.move(game.avai_op[rand_choice(rg)]);
  }
  return (game.status - 1) / 2.0;
}

void NodeBackprop(double result, unsigned simul_count, Node* node) {
  while (node != nullptr) {
    node->simul_count += simul_count;
    node->win_count += result;
    node = node->p_node;
  }
}

void NodeWithLockBackprop(double result, unsigned simul_count,
                          NodeWithLock* node) {
  while (node != nullptr) {
    node->simul_count += simul_count;
    double wc, nwc;
    do {
      wc = node->win_count;
      nwc = wc + result;
    } while (!(node->win_count.compare_exchange_weak(wc, nwc)));
    node = node->p_node;
  }
}

MCTS::MCTS(unsigned board_size) {
  root = new Node(board_size);
  std::random_device rd;
  rg.seed(rd());
}

MCTS::~MCTS() { delete root; }

void MCTS::UCTSearch(int round) {
  for (int i = 0; i < round; i++) {
    Node* node = NodeExpand(NodeSelect(root), rg);
    NodeBackprop(Simulate(node->game, rg), 1, node);
  }
}

void MCTS::UpdateTree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  Node* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new Node(root, move);
  } else {
    int move_node = FindMove(move, root->game.avai_op);
    new_node = root->c_node[move_node];
    root->c_node[move_node] = nullptr;
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS::GetMove() {
  double max_val = 0;
  int next = 0;
  Node** c_node = root->c_node;
  for (int i = 0; i < root->c_node_size; i++) {
    if (c_node[i] == nullptr) {
      continue;
    }
    double score = c_node[i]->win_count / (double)(c_node[i]->simul_count);
    if (c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return root->game.avai_op[next];
}

void MCTS_L_THD_Worker(Node* node, double* result, std::mt19937* rg, int load) {
  *result = 0;
  for (int i = 0; i < load; i++) {
    *result += Simulate(node->game, *rg);
  }
}

MCTS_L_THD::MCTS_L_THD(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), thds(thread_num), rgs(thread_num), results(thread_num) {
  root = new Node(board_size);
  load = 4;
  std::random_device rd;
  rg.seed(rd());
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rg());
  }
}

MCTS_L_THD::~MCTS_L_THD() { delete root; }

void MCTS_L_THD::UCTSearch(int round) {
  for (int j = 0; j < (round / (thdn * load)); j++) {
    Node* node = NodeExpand(NodeSelect(root), rg);
    for (int i = 0; i < thdn; i++) {
      thds[i] =
          std::thread(MCTS_L_THD_Worker, node, &(results[i]), &(rgs[i]), load);
    }
    for (int i = 0; i < thdn; i++) {
      thds[i].join();
    }
    double sum = 0;
    for (int i = 0; i < thdn; i++) {
      sum += results[i];
    }
    NodeBackprop(sum, thdn * load, node);
  }
}

void MCTS_L_THD::UpdateTree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  Node* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new Node(root, move);
  } else {
    int move_node = FindMove(move, root->game.avai_op);
    new_node = root->c_node[move_node];
    root->c_node[move_node] = nullptr;
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS_L_THD::GetMove() {
  double max_val = 0;
  int next = 0;
  Node** c_node = root->c_node;
  for (int i = 0; i < root->c_node_size; i++) {
    if (c_node[i] == nullptr) {
      continue;
    }
    double score = c_node[i]->win_count / (double)(c_node[i]->simul_count);
    if (c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return root->game.avai_op[next];
}

MCTS_R_THD::MCTS_R_THD(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), thds(thread_num), rgs(thread_num), roots(thread_num) {
  for (int i = 0; i < thdn; i++) {
    roots[i] = new Node(board_size);
  }
  std::random_device rd;
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rd());
  }
}

MCTS_R_THD::~MCTS_R_THD() {
  for (int i = 0; i < thdn; i++) {
    delete roots[i];
  }
}

void MCTS_R_THD_Worker(Node* root, std::mt19937* rg, int round) {
  for (int i = 0; i < round; i++) {
    Node* node = NodeExpand(NodeSelect(root), *rg);
    NodeBackprop(Simulate(node->game, *rg), 1, node);
  }
}

void MCTS_R_THD::UCTSearch(int round) {
  for (int i = 0; i < thdn; i++) {
    thds[i] = std::thread(MCTS_R_THD_Worker, roots[i], &(rgs[i]), round / thdn);
  }
  for (int i = 0; i < thdn; i++) {
    thds[i].join();
  }
}

void MCTS_R_THD::UpdateTree(int move) {
  for (auto& root : roots) {
    std::vector<int>::iterator find_move =
        std::find(root->untry_op.begin(), root->untry_op.end(), move);
    Node* new_node;
    if (find_move != root->untry_op.end()) {
      new_node = new Node(root, move);
    } else {
      int move_node = FindMove(move, root->game.avai_op);
      new_node = root->c_node[move_node];
      root->c_node[move_node] = nullptr;
    }
    delete root;
    root = new_node;
    root->p_node = nullptr;
  }
}

int MCTS_R_THD::GetMove() {
  double max_val = 0;
  int next = 0;
  for (int i = 0; i < roots[0]->c_node_size; i++) {
    unsigned simul_count = 0;
    double win_count = 0;
    int val = 0;
    for (int j = 0; j < thdn; j++) {
      if (roots[j]->c_node[i] == nullptr) {
        continue;
      }
      val = j;
      simul_count += roots[j]->c_node[i]->simul_count;
      win_count += roots[j]->c_node[i]->win_count;
    }
    if (simul_count == 0) continue;
    double score = win_count / (double)(simul_count);
    if (roots[val]->c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return roots[0]->game.avai_op[next];
}

MCTS_T_G_THD::MCTS_T_G_THD(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), thds(thread_num), rgs(thread_num) {
  root = new Node(board_size);
  std::random_device rd;
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rd());
  }
}

MCTS_T_G_THD::~MCTS_T_G_THD() { delete root; }

void MCTS_T_G_THD_Worker(Node* root, std::mt19937* rg, int round,
                         std::mutex* lck_exp, std::mutex* lck_bkp) {
  for (int i = 0; i < round; i++) {
    Node* node = NodeSelect(root);
    while (true) {
      lck_exp->lock();
      if ((!(node->tried_all())) || (node->c_node_size == 0)) {
        break;
      } else {
        lck_exp->unlock();
        node = NodeSelect(node);
      }
    }
    node = NodeExpand(node, *rg);
    lck_exp->unlock();
    double r = Simulate(node->game, *rg);
    lck_bkp->lock();
    NodeBackprop(r, 1, node);
    lck_bkp->unlock();
  }
}

void MCTS_T_G_THD::UCTSearch(int round) {
  for (int i = 0; i < thdn; i++) {
    thds[i] = std::thread(MCTS_T_G_THD_Worker, root, &(rgs[i]), round / thdn,
                          &lck_exp, &lck_bkp);
  }
  for (int i = 0; i < thdn; i++) {
    thds[i].join();
  }
}

void MCTS_T_G_THD::UpdateTree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  Node* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new Node(root, move);
  } else {
    int move_node = FindMove(move, root->game.avai_op);
    new_node = root->c_node[move_node];
    root->c_node[move_node] = nullptr;
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS_T_G_THD::GetMove() {
  double max_val = 0;
  int next = 0;
  Node** c_node = root->c_node;
  for (int i = 0; i < root->c_node_size; i++) {
    if (c_node[i] == nullptr) {
      continue;
    }
    double score = c_node[i]->win_count / (double)(c_node[i]->simul_count);
    if (c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return root->game.avai_op[next];
}

MCTS_T_L_THD::MCTS_T_L_THD(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), thds(thread_num), rgs(thread_num) {
  root = new NodeWithLock(board_size);
  std::random_device rd;
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rd());
  }
}

MCTS_T_L_THD::~MCTS_T_L_THD() { delete root; }

void MCTS_T_L_THD_Worker(NodeWithLock* root, std::mt19937* rg, int round) {
  for (int i = 0; i < round; i++) {
    NodeWithLock* node = NodeWithLockSelect(root);
    while (true) {
      node->lck.lock();
      if ((!(node->tried_all())) || (node->c_node_size == 0)) {
        break;
      } else {
        node->lck.unlock();
        node = NodeWithLockSelect(node);
      }
    }
    NodeWithLock* nnode = NodeWithLockExpand(node, *rg);
    node->lck.unlock();
    double r = Simulate(nnode->game, *rg);
    NodeWithLockBackprop(r, 1, nnode);
  }
}

void MCTS_T_L_THD::UCTSearch(int round) {
  for (int i = 0; i < thdn; i++) {
    thds[i] = std::thread(MCTS_T_L_THD_Worker, root, &(rgs[i]), round / thdn);
  }
  for (int i = 0; i < thdn; i++) {
    thds[i].join();
  }
}

void MCTS_T_L_THD::UpdateTree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  NodeWithLock* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new NodeWithLock(root, move);
  } else {
    int move_node = FindMove(move, root->game.avai_op);
    new_node = root->c_node[move_node];
    root->c_node[move_node] = nullptr;
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS_T_L_THD::GetMove() {
  double max_val = 0;
  int next = 0;
  NodeWithLock** c_node = root->c_node;
  for (int i = 0; i < root->c_node_size; i++) {
    if (c_node[i] == nullptr) {
      continue;
    }
    double score = c_node[i]->win_count / (double)(c_node[i]->simul_count);
    if (c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return root->game.avai_op[next];
}



void MCTS_L_OMP_Worker(Node* node, double* result, std::mt19937* rg, int load) {
  *result = 0;
  for (int i = 0; i < load; i++) {
    *result += Simulate(node->game, *rg);
  }
}

MCTS_L_OMP::MCTS_L_OMP(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), rgs(thread_num) {
  root = new Node(board_size);
  load = 4;
  std::random_device rd;
  rg.seed(rd());
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rg());
  }
  omp_set_dynamic(0);
  omp_set_num_threads(thread_num);
}

MCTS_L_OMP::~MCTS_L_OMP() { delete root; }

void MCTS_L_OMP::UCTSearch(int round) {
  for (int j = 0; j < (round / (thdn * load)); j++) {
    Node* node = NodeExpand(NodeSelect(root), rg);
    double sum = 0;
#pragma omp parallel shared(node, rgs, load) reduction(+ : sum)
    {    
      int i = omp_get_thread_num();
      MCTS_L_OMP_Worker(node, &sum, &(rgs[i]), load);
    }
    NodeBackprop(sum, thdn * load, node);
  }
}

void MCTS_L_OMP::UpdateTree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  Node* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new Node(root, move);
  } else {
    int move_node = FindMove(move, root->game.avai_op);
    new_node = root->c_node[move_node];
    root->c_node[move_node] = nullptr;
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS_L_OMP::GetMove() {
  double max_val = 0;
  int next = 0;
  Node** c_node = root->c_node;
  for (int i = 0; i < root->c_node_size; i++) {
    if (c_node[i] == nullptr) {
      continue;
    }
    double score = c_node[i]->win_count / (double)(c_node[i]->simul_count);
    if (c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return root->game.avai_op[next];
}



MCTS_R_OMP::MCTS_R_OMP(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), rgs(thread_num), roots(thread_num) {
  for (int i = 0; i < thdn; i++) {
    roots[i] = new Node(board_size);
  }
  std::random_device rd;
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rd());
  }
  omp_set_dynamic(0);
  omp_set_num_threads(thread_num);
}

MCTS_R_OMP::~MCTS_R_OMP() {
  for (int i = 0; i < thdn; i++) {
    delete roots[i];
  }
}

void MCTS_R_OMP_Worker(Node* root, std::mt19937* rg, int round) {
  for (int i = 0; i < round; i++) {
    Node* node = NodeExpand(NodeSelect(root), *rg);
    NodeBackprop(Simulate(node->game, *rg), 1, node);
  }
}

void MCTS_R_OMP::UCTSearch(int round) {
  #pragma omp parallel shared(roots, rgs, round, thdn)
  {    
    int i = omp_get_thread_num();
    MCTS_R_OMP_Worker(roots[i], &(rgs[i]), round / thdn);
  }
}

void MCTS_R_OMP::UpdateTree(int move) {
  for (auto& root : roots) {
    std::vector<int>::iterator find_move =
        std::find(root->untry_op.begin(), root->untry_op.end(), move);
    Node* new_node;
    if (find_move != root->untry_op.end()) {
      new_node = new Node(root, move);
    } else {
      int move_node = FindMove(move, root->game.avai_op);
      new_node = root->c_node[move_node];
      root->c_node[move_node] = nullptr;
    }
    delete root;
    root = new_node;
    root->p_node = nullptr;
  }
}

int MCTS_R_OMP::GetMove() {
  double max_val = 0;
  int next = 0;
  for (int i = 0; i < roots[0]->c_node_size; i++) {
    unsigned simul_count = 0;
    double win_count = 0;
    int val = 0;
    for (int j = 0; j < thdn; j++) {
      if (roots[j]->c_node[i] == nullptr) {
        continue;
      }
      val = j;
      simul_count += roots[j]->c_node[i]->simul_count;
      win_count += roots[j]->c_node[i]->win_count;
    }
    if (simul_count == 0) continue;
    double score = win_count / (double)(simul_count);
    if (roots[val]->c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return roots[0]->game.avai_op[next];
}


MCTS_T_L_OMP::MCTS_T_L_OMP(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), rgs(thread_num) {
  root = new NodeWithLock(board_size);
  std::random_device rd;
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rd());
  }
  omp_set_dynamic(0);
  omp_set_num_threads(thread_num);
}

MCTS_T_L_OMP::~MCTS_T_L_OMP() { delete root; }

void MCTS_T_L_OMP_Worker(NodeWithLock* root, std::mt19937* rg, int round) {
  for (int i = 0; i < round; i++) {
    NodeWithLock* node = NodeWithLockSelect(root);
    while (true) {
      node->lck.lock();
      if ((!(node->tried_all())) || (node->c_node_size == 0)) {
        break;
      } else {
        node->lck.unlock();
        node = NodeWithLockSelect(node);
      }
    }
    NodeWithLock* nnode = NodeWithLockExpand(node, *rg);
    node->lck.unlock();
    double r = Simulate(nnode->game, *rg);
    NodeWithLockBackprop(r, 1, nnode);
  }
}

void MCTS_T_L_OMP::UCTSearch(int round) {
#pragma omp parallel shared(root, rgs, round, thdn)
  {    
    int i = omp_get_thread_num();
    MCTS_T_L_THD_Worker(root, &(rgs[i]), round / thdn);
  }
}

void MCTS_T_L_OMP::UpdateTree(int move) {
  std::vector<int>::iterator find_move =
      std::find(root->untry_op.begin(), root->untry_op.end(), move);
  NodeWithLock* new_node;
  if (find_move != root->untry_op.end()) {
    new_node = new NodeWithLock(root, move);
  } else {
    int move_node = FindMove(move, root->game.avai_op);
    new_node = root->c_node[move_node];
    root->c_node[move_node] = nullptr;
  }
  delete root;
  root = new_node;
  root->p_node = nullptr;
}

int MCTS_T_L_OMP::GetMove() {
  double max_val = 0;
  int next = 0;
  NodeWithLock** c_node = root->c_node;
  for (int i = 0; i < root->c_node_size; i++) {
    if (c_node[i] == nullptr) {
      continue;
    }
    double score = c_node[i]->win_count / (double)(c_node[i]->simul_count);
    if (c_node[i]->game.player != 0) {
      score = 1.0 - score;
    }
    if (score > max_val) {
      max_val = score;
      next = i;
    }
  }
  return root->game.avai_op[next];
}