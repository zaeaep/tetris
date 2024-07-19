#pragma once

#include "TerminalManager.h"
#include "Tetris.h"
#include "Tetromino.h"
#include <array>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>

class Tetromino {
public:
  Tetromino(std::vector<std::vector<std::vector<int>>> shapes, int color);

  void rotateClockwise(const std::vector<std::vector<int>> &field);

  void rotateCounterClockwise(const std::vector<std::vector<int>> &field);

  // funktion die die shape des Tetrominos zurückgibt
  const std::vector<std::vector<int>> &getShape() const;

  // Methode die die Farbe des Tetrominos zurückgibt
  int getColor() const;

  // Methode die das Tetromino auf dem Sielfeld bewegt
  bool move(int dx, int dy, const std::vector<std::vector<int>> &field);

  // Methode zum setzen der Position des Tetrominos
  void setPosition(int r, int c);

  // Zeichnen des nexten Tetrominos neben dem Feld
  void drawNextTetromino(TerminalManager &terminal, int row, int col, int width,
                         int height) const;

  // Methode die die aktuelle Position des Tetroinos zurückgibt
  std::pair<int, int> getPosition() const;

private:
  bool isValidPosition(int r, int c, int s,
                       const std::vector<std::vector<int>> &field) const;
  std::vector<std::vector<std::vector<int>>> shapes;
  int color;
  int state;
  int row;
  int col;
  // int currentShapeIndex_;
};

// Klasse, die alle Tetrominos und deren eigenschaften enthält

class Tetrominos {
public:
  Tetrominos();

  // Methode die einen zufälligen Tetromino zurückgibt
  Tetromino getRandomTetromino();

private:
  std::vector<std::vector<std::vector<std::vector<int>>>> tetrominoShapes;
  std::vector<int> tetrominoColors;
  int lastIndex;
  // void initializeTetrominos();
};

void updateTetromino(Tetromino &currentTetromino, Tetrominos &tetrominos,
                     std::vector<std::vector<int>> &field,
                     std::chrono::steady_clock::time_point &lastTick,
                     bool &running, const int FIELD_WIDTH, int &TICK_RATE_MS,
                     int &totalLinesCleared, int &linesClearedAtOnce,
                     Tetromino &nextTetromino, TerminalManager &terminal,
                     int &level);

void placeTetrominoInField(std::vector<std::vector<int>> &field,
                           const Tetromino &tetromino);

void checkAndRemoveFullLines(std::vector<std::vector<int>> &field,
                             int &totalLinesCleared, int &linesClearedAtOnce);

// void drawNextTetromino(TerminalManager &terminal, int row, int col);
int calculateTickRate(int level);
