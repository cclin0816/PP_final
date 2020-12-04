#include "game.hpp"

#include <algorithm>

Game::Game(unsigned _size)
    : size(_size), board(_size * _size, -1), avai_op(_size * _size) {
  board.shrink_to_fit();
  avai_op.shrink_to_fit();
  for (int i = 0; i < _size * _size; ++i) {
    avai_op[i] = i;
  }
}

Game::Game(const Game& game)
    : size(game.size),
      board(game.board),
      avai_op(game.avai_op),
      prev_move(game.prev_move),
      player(game.player),
      status(game.status) {
  board.shrink_to_fit();
  avai_op.shrink_to_fit();
}

Game::~Game() {}

void Game::move(int pos) {
  // put piece
  board[pos] = player;
  prev_move = pos;
  if (bingo(pos)) {
    status = 1 + player * 2;  // win
    avai_op.clear();
    avai_op.shrink_to_fit();
  } else {
    avai_op.erase(find(avai_op.begin(), avai_op.end(), pos));
    if (avai_op.size() == 0) {
      status = 2;  // tie
      avai_op.shrink_to_fit();
    }
  }
  // switch player
  player ^= 1;
}

bool Game::bingo(int pos) {
  // left right
  int line_len = 1;
  int pos_itr = pos;
  while (pos_itr % size > 0) {
    pos_itr--;
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  pos_itr = pos;
  while (pos_itr % size < size - 1) {
    pos_itr++;
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  if (line_len >= 5) {
    return true;
  }
  // up down
  line_len = 1;
  pos_itr = pos;
  while (pos_itr / size > 0) {
    pos_itr -= size;
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  pos_itr = pos;
  while (pos_itr / size < size - 1) {
    pos_itr += size;
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  if (line_len >= 5) {
    return true;
  }
  // left up right down
  line_len = 1;
  pos_itr = pos;
  while (pos_itr / size > 0 && pos_itr % size > 0) {
    pos_itr -= (size + 1);
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  pos_itr = pos;
  while (pos_itr / size < size - 1 && pos_itr % size < size - 1) {
    pos_itr += (size + 1);
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  if (line_len >= 5) {
    return true;
  }
  // right up left down
  line_len = 1;
  pos_itr = pos;
  while (pos_itr / size > 0 && pos_itr % size < size - 1) {
    pos_itr -= (size - 1);
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  pos_itr = pos;
  while (pos_itr / size < size - 1 && pos_itr % size > 0) {
    pos_itr += (size - 1);
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  if (line_len >= 5) {
    return true;
  }
  return false;
}