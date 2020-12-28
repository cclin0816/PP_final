#include "mcts.hpp"

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/thread/future.hpp>
#include <boost/asio/post.hpp>

Node* NodeSelect(Node* root);
int FindMove(int move, std::vector<int>& avai_op);
Node* NodeExpand(Node* node, std::mt19937& rg);
void NodeBackprop(double result, unsigned simul_count, Node* node);

double Simpool(Game* g, std::mt19937 *rg) {
  Game game(*g);
  while (game.status == 0) {
    std::uniform_int_distribution<int> rand_choice(0, game.avai_op.size() - 1);
    game.move(game.avai_op[rand_choice(*rg)]);
  }
  return (game.status - 1) / 2.0;
}

MCTS_L_TP::MCTS_L_TP(unsigned board_size, unsigned thread_num)
    : thdn(thread_num), rgs(thread_num), pool(thread_num) {
  root = new Node(board_size);
  load = 4;
  std::random_device rd;
  rg.seed(rd());
  for (int i = 0; i < thdn; i++) {
    rgs[i].seed(rg());
  }
}

MCTS_L_TP::~MCTS_L_TP() { 
  delete root;
}

void MCTS_L_TP::UCTSearch(int round) {
  for (int j = 0; j < (round / (thdn)); j++) {
    Node* node = NodeExpand(NodeSelect(root), rg);
    double sum = 0;

    std::vector<boost::packaged_task<double>> tasks;

    for(int i = 0; i < thdn; i++)
        tasks.emplace_back(boost::bind(Simpool, &(node->game), &(rgs[i])));

    std::vector<boost::unique_future<boost::packaged_task<double>::result_type> > futures;
    
    for (auto& t : tasks) {
        futures.push_back(t.get_future());
        boost::asio::post(pool, std::move(t));
    }
    for (auto& fut : futures) {
        sum += fut.get();
    }
    NodeBackprop(sum, thdn, node);
  }
}

void MCTS_L_TP::UpdateTree(int move) {
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

int MCTS_L_TP::GetMove() {
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