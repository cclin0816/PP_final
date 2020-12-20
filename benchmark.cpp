#include <chrono>
#include <iostream>
#include <vector>

#include "mcts.hpp"

void benchmark(M *ref, M *subject, std::vector<long long> &time_record,
               std::vector<int> &time_count, int first, double &score,
               unsigned board_size, int sim_count) {
  M *player[2];
  player[0] = ref;
  player[1] = subject;
  int move_count = 0;
  Game game(board_size);
  while (game.status == 0) {
    int now_play = (move_count + first) % 2;
    auto t1 = std::chrono::high_resolution_clock::now();
    player[now_play]->UCTSearch(sim_count);
    int move = player[now_play]->GetMove();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    if (now_play) {
      time_record[move_count] += duration;
      time_count[move_count]++;
    }
    game.move(move);
    player[0]->UpdateTree(move);
    player[1]->UpdateTree(move);
    move_count++;
  }
  double s = (double)(game.status - 1) / 2.0;
  if (first) {
    s = 1 - s;
  }
  score += s;
  std::cout << s << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::vector<long long> time_record(13 * 13, 0);
  std::vector<int> time_count(13 * 13, 0);
  double score = 0;
  for (int i = 0; i < 50; i++) {
    MCTS p1(13);
    MCTS_T_G_THD p2(13, 4);
    benchmark(&p1, &p2, time_record, time_count, i % 2, score, 13, 4096);
  }
  std::cout << score / 50 << std::endl;
  for(int i = 0; i < 13 * 13; i++) {
    if(time_count[i] != 0)
      std::cout << i << ' ' << time_record[i] / time_count[i] << '\n';
  }
}