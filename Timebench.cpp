#include <stdlib.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "define.hpp"
#include "mcts.hpp"

// using namespace std;

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
  ref.close();
}
void write_time_ref(const char *target, std::vector<long long> &time_record,
                    std::vector<int> &time_count, int boardsize) {
  std::string name = target;
  name += ".time";
  std::fstream ref(name.c_str(), std::fstream::out);
  for (int i = 0; i < boardsize * boardsize; i++) {
    if (time_count[i] != 0) {
      ref << time_record[i] / time_count[i] << '\n';
    } else {
      break;
    }
  }
  ref.close();
}

void benchtime(const char *target, std::vector<long long> &time_record,
               std::vector<int> &time_count, int boardsize,
               std::vector<int> &simcount_ref) {
  M *agent[2];
  for (int i = 0; i < 2; i++) {
    agent[i] = new_agent(target, boardsize);
  }
  int move_count = 0;
  Game game(boardsize);
  while (game.status == 0) {
    auto t1 = std::chrono::high_resolution_clock::now();
    agent[move_count % 2]->UCTSearch(simcount_ref[move_count]);
    int move = agent[move_count % 2]->GetMove();
    agent[move_count % 2]->UpdateTree(move);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    time_record[move_count] += duration;
    time_count[move_count]++;
    game.move(move);
    agent[++move_count % 2]->UpdateTree(move);
  }
  for (int i = 0; i < 2; i++) {
    del_agent(target, agent[i]);
  }
}

int main(int argc, char **argv) {
  std::ios_base::sync_with_stdio(false);
  int boardsize = atoi(argv[2]);
  int round = atoi(argv[3]);
  std::vector<long long> time_record(boardsize * boardsize, 0);
  std::vector<int> time_count(boardsize * boardsize, 0);
  std::vector<int> simcount_ref(boardsize * boardsize, 0);
  get_simcount_ref(argv[1], simcount_ref, boardsize);
  for (int i = 0; i < round; i++) {
    benchtime(argv[1], time_record, time_count, boardsize, simcount_ref);
  }
  write_time_ref(argv[1], time_record, time_count, boardsize);
}