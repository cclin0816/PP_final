#include "game.hpp"

#include <algorithm>
#include <iostream>

Game::Game(unsigned board_size)
    : _board_size(board_size),
      avai_op(board_size * board_size),
      player(0),
      status(0) {
  board = new int8_t[_board_size * _board_size];
  for (int i = 0; i < _board_size * _board_size; ++i) {
    board[i] = -1;
  }
  avai_op.shrink_to_fit();
  for (int i = 0; i < _board_size * _board_size; ++i) {
    avai_op[i] = i;
  }
}

Game::Game(const Game& game)
    : _board_size(game._board_size),
      avai_op(game.avai_op),
      player(game.player),
      status(game.status) {
  board = new int8_t[_board_size * _board_size];
  for (int i = 0; i < _board_size * _board_size; ++i) {
    board[i] = game.board[i];
  }
  avai_op.shrink_to_fit();
}

Game::~Game() {
  delete[] board;
}

void Game::move(int pos) {
  // put piece
  board[pos] = player;
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
  int size = _board_size;
  int size_1 = size - 1;
  while (pos_itr % size > 0) {
    pos_itr--;
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  pos_itr = pos;
  while (pos_itr % size < size_1) {
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
  while (pos_itr / size < size_1) {
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
  while (pos_itr / size < size_1 && pos_itr % size < size_1) {
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
  while (pos_itr / size > 0 && pos_itr % size < size_1) {
    pos_itr -= (size_1);
    if (board[pos_itr] == player) {
      line_len++;
    } else {
      break;
    }
  }
  pos_itr = pos;
  while (pos_itr / size < size_1 && pos_itr % size > 0) {
    pos_itr += (size_1);
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