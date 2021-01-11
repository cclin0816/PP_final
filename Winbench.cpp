#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "define.hpp"
#include "mcts.hpp"


M *new_agent(const char *target, int boardsize) {
  std::string name = target;
  if (name == "MCTS") {
    return new MCTS(boardsize);
  } else if (name == "MCTS_L_THD") {
    return new MCTS_L_THD(boardsize, thread_count);
  } else if (name == "MCTS_R_THD") {
    return new MCTS_R_THD(boardsize, thread_count);
  } else if (name == "MCTS_T_G_THD") {
    return new MCTS_T_G_THD(boardsize, thread_count);
  } else if (name == "MCTS_T_L_THD") {
    return new MCTS_T_L_THD(boardsize, thread_count);
  } else if (name == "MCTS_L_OMP") {
    return new MCTS_L_OMP(boardsize, thread_count);
  } else if (name == "MCTS_R_OMP") {
    return new MCTS_R_OMP(boardsize, thread_count);
  } else if (name == "MCTS_T_L_OMP") {
    return new MCTS_T_L_OMP(boardsize, thread_count);
  }
  return nullptr;
}

void del_agent(const char *target, M *agent) {
  std::string name = target;
  if (name == "MCTS") {
    MCTS *a = (MCTS *)agent;
    delete a;
  } else if (name == "MCTS_L_THD") {
    MCTS_L_THD *a = (MCTS_L_THD *)agent;
    delete a;
  } else if (name == "MCTS_R_THD") {
    MCTS_R_THD *a = (MCTS_R_THD *)agent;
    delete a;
  } else if (name == "MCTS_T_G_THD") {
    MCTS_T_G_THD *a = (MCTS_T_G_THD *)agent;
    delete a;
  } else if (name == "MCTS_T_L_THD") {
    MCTS_T_L_THD *a = (MCTS_T_L_THD *)agent;
    delete a;
  } else if (name == "MCTS_L_OMP") {
    MCTS_L_OMP *a = (MCTS_L_OMP *)agent;
    delete a;
  } else if (name == "MCTS_R_OMP") {
    MCTS_R_OMP *a = (MCTS_R_OMP *)agent;
    delete a;
  } else if (name == "MCTS_T_L_OMP") {
    MCTS_T_L_OMP *a = (MCTS_T_L_OMP *)agent;
    delete a;
  }
}

void get_simcount_ref(const char *target, std::vector<int> &simcount_ref,
                      int boardsize) {
  std::string name = target;
  name += ".conf";
  std::fstream ref(name.c_str(), std::fstream::in);
  for (int i = 0; i < boardsize * boardsize; i++) {
    ref >> simcount_ref[i];
  }
}
void write_win_ref(const char *target, double score, int round) {
  std::string name = target;
  name += ".win";
  std::fstream refi(name.c_str(), std::fstream::in);
  int prev_round;
  double prev_score;
  refi >> prev_round >> prev_score;
  refi.close();
  std::fstream refo(name.c_str(), std::fstream::out);
  refo << round + prev_round << ' ' << (score + prev_round * prev_score) / (round + prev_round) << '\n';
  std::cout << "score: " << (score + prev_round * prev_score) / (round + prev_round) << std::endl;
}

double benchwin(const char *target, int boardsize,
               std::vector<int> &simcount_ref, std::vector<int> &simcount_MCTS) {
  double score = 0;
  M *agent[2];
  agent[0] = new_agent(target, boardsize);
  agent[1] = new_agent("MCTS", boardsize);
  int move_count = 0;
  Game game(boardsize);
  while (game.status == 0) {
    agent[0]->UCTSearch(simcount_ref[move_count]);
    int move = agent[0]->GetMove();
    agent[0]->UpdateTree(move);
    game.move(move);
    agent[1]->UpdateTree(move);
    move_count++;
    if(game.status != 0) break;
    agent[1]->UCTSearch(simcount_MCTS[move_count]);
    move = agent[1]->GetMove();
    agent[1]->UpdateTree(move);
    game.move(move);
    agent[0]->UpdateTree(move);
    move_count++;
  }
  del_agent(target, agent[0]);
  del_agent("MCTS", agent[1]);
  score = 1.0 - (game.status - 1) / 2.0;
  agent[1] = new_agent(target, boardsize);
  agent[0] = new_agent("MCTS", boardsize);
  move_count = 0;
  Game game2(boardsize);
  while (game2.status == 0) {
    agent[0]->UCTSearch(simcount_MCTS[move_count]);
    int move = agent[0]->GetMove();
    agent[0]->UpdateTree(move);
    game2.move(move);
    agent[1]->UpdateTree(move);
    move_count++;
    if(game2.status != 0) break;
    agent[1]->UCTSearch(simcount_ref[move_count]);
    move = agent[1]->GetMove();
    agent[1]->UpdateTree(move);
    game2.move(move);
    agent[0]->UpdateTree(move);
    move_count++;
  }
  del_agent(target, agent[1]);
  del_agent("MCTS", agent[0]);
  score += ((game2.status - 1) / 2.0);
  return score / 2.0;
}

int main(int argc, char **argv) {
  std::ios_base::sync_with_stdio(false);
  int boardsize = atoi(argv[2]);
  int round = atoi(argv[3]);
  std::vector<int> simcount_ref(boardsize * boardsize, 0);
  get_simcount_ref(argv[1], simcount_ref, boardsize);
  std::vector<int> simcount_MCTS(boardsize * boardsize, 0);
  get_simcount_ref("MCTS", simcount_MCTS, boardsize);
  double score = 0;
  for (int i = 0; i < round; i++) {
    score += benchwin(argv[1], boardsize, simcount_ref, simcount_MCTS);
  }
  write_win_ref(argv[1], score, round);
}