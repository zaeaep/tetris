#include "Tetromino.h"
#include "TerminalManager.h"
#include <algorithm>
#include <random>

const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 20;
const int BORDER_COLOR = 1;
const int LEVEL_SPEEDS[] = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6,
                            5,  5,  5,  4,  4,  4,  3,  3,  3, 2,
                            2,  2,  2,  2,  2,  2,  2,  2,  2, 1};
const int FRAMES_PER_SECOND = 60;

Tetromino::Tetromino(std::vector<std::vector<std::vector<int>>> shapes,
                     int color)
    : shapes(shapes), color(color), state(0), row(0), col(0) {}

void Tetromino::rotateClockwise(const std::vector<std::vector<int>> &field) {
  int newState = (state + 1) % shapes.size();
  if (isValidPosition(row, col, newState, field)) {
    state = newState;
  }
}

void Tetromino::rotateCounterClockwise(
    const std::vector<std::vector<int>> &field) {
  int newState = (state - 1 + shapes.size()) % shapes.size();
  if (isValidPosition(row, col, newState, field)) {
    state = newState;
  }
}

const std::vector<std::vector<int>> &Tetromino::getShape() const {
  return shapes[state];
}

int Tetromino::getColor() const { return color; }

bool Tetromino::move(int dx, int dy,
                     const std::vector<std::vector<int>> &field) {
  int newRow = row + dy;
  int newCol = col + dx;

  if (isValidPosition(newRow, newCol, state, field)) {
    row = newRow;
    col = newCol;
    return true;
  }
  return false;
}

bool Tetromino::isValidPosition(
    int r, int c, int s, const std::vector<std::vector<int>> &field) const {
  const auto &shape = shapes[s];
  for (int i = 0; i < shape.size(); i++) {
    for (int j = 0; j < shape[i].size(); j++) {
      if (shape[i][j] != 0) {
        int newRow = r + i;
        int newCol = c + j;
        if (newRow < 0 || newRow >= field.size() || newCol < 0 ||
            newCol >= field[0].size() || field[newRow][newCol] != 0) {
          return false;
        }
      }
    }
  }
  return true;
}

void Tetromino::setPosition(int r, int c) {
  row = r;
  col = c;
}

std::pair<int, int> Tetromino::getPosition() const { return {row, col}; }

// Tetrominos::Tetrominos() { initializeTetrominos(); }

Tetrominos::Tetrominos() : lastIndex(-1) {
  tetrominoShapes = {
      {{{1, 1, 1, 1}}, {{1}, {1}, {1}, {1}}}, // I
      {{{1, 1, 1}, {0, 1, 0}},
       {{0, 1}, {1, 1}, {0, 1}},
       {{0, 1, 0}, {1, 1, 1}},
       {{1, 0}, {1, 1}, {1, 0}}}, // T
      {{{1, 1, 1}, {1, 0, 0}},
       {{1, 1}, {0, 1}, {0, 1}},
       {{0, 0, 1}, {1, 1, 1}},
       {{1, 0}, {1, 0}, {1, 1}}}, // L
      {{{1, 1, 1}, {0, 0, 1}},
       {{0, 1}, {0, 1}, {1, 1}},
       {{1, 0, 0}, {1, 1, 1}},
       {{1, 1}, {1, 0}, {1, 0}}},                         // J
      {{{1, 1}, {1, 1}}},                                 // O
      {{{0, 1, 1}, {1, 1, 0}}, {{1, 0}, {1, 1}, {0, 1}}}, // S
      {{{1, 1, 0}, {0, 1, 1}}, {{0, 1}, {1, 1}, {1, 0}}}  // Z
  };
  tetrominoColors = {2, 3, 4, 5, 6, 7, 8};
  std::srand(std::time(nullptr));
}

Tetromino Tetrominos::getRandomTetromino() {
  int index;
  do {
    index = std::rand() % tetrominoShapes.size();
  } while (index == lastIndex && std::rand() % 7 == 0);
  lastIndex = index;
  // int randomColor = tetrominoColors[std::rand() % tetrominoColors.size()];
  return Tetromino(tetrominoShapes[index], tetrominoColors[index]);
}

void updateTetromino(Tetromino &currentTetromino, Tetrominos &tetrominos,
                     std::vector<std::vector<int>> &field,
                     std::chrono::steady_clock::time_point &lastTick,
                     bool &exitRequested, const int FIELD_WIDTH,
                     int &TICK_RATE_MS, int &totalLinesCleared,
                     int &linesClearedAtOnce, Tetromino &nextTetromino,
                     TerminalManager &terminal, int &level) {
  auto now = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = now - lastTick;
  bool shouldMoveDown = elapsed.count() * FRAMES_PER_SECOND >= TICK_RATE_MS;

  if (shouldMoveDown) {
    lastTick = now;
    if (!currentTetromino.move(0, 1, field)) {
      placeTetrominoInField(field, currentTetromino);
      checkAndRemoveFullLines(field, totalLinesCleared, linesClearedAtOnce);
      currentTetromino = nextTetromino; // Verwende das nächste Tetromino
      currentTetromino.setPosition(
          0, FIELD_WIDTH / 2 - currentTetromino.getShape()[0].size() / 2);
      nextTetromino =
          tetrominos
              .getRandomTetromino(); // Generiere ein neues nächstes Tetromino
      nextTetromino.drawNextTetromino(terminal, 2, FIELD_WIDTH + 5, 4, 4);
      if (!currentTetromino.move(0, 0, field)) {
        exitRequested = true;
      }
      nextTetromino.drawNextTetromino(terminal, 2, FIELD_WIDTH + 5, 4, 4);
    }
  }
  int previousLevel = level;
  level = totalLinesCleared / 10;
  if (level != previousLevel) {
    TICK_RATE_MS = calculateTickRate(level);
  }
}

void placeTetrominoInField(std::vector<std::vector<int>> &field,
                           const Tetromino &tetromino) {
  const auto &shape = tetromino.getShape();
  auto [startRow, startCol] = tetromino.getPosition();
  int color = tetromino.getColor();
  for (int i = 0; i < shape.size(); ++i) {
    for (int j = 0; j < shape[i].size(); ++j) {
      if (shape[i][j] != 0) {
        field[startRow + i][startCol + j] = color;
      }
    }
  }
}

void checkAndRemoveFullLines(std::vector<std::vector<int>> &field,
                             int &totalLinesCleared, int &linesClearedAtOnce) {
  linesClearedAtOnce = 0;

  for (int row = field.size() - 1; row >= 0; row--) {
    bool isFullLine = true;
    for (int col = 0; col < field[row].size(); col++) {
      if (field[row][col] == 0) {
        isFullLine = false;
        break;
      }
    }
    if (isFullLine) {
      linesClearedAtOnce++;
      totalLinesCleared++;

      for (int r = row; r > 0; r--) {
        field[r] = field[r - 1];
      }

      field[0] = std::vector<int>(field[0].size(), 0);

      row++;
    }
  }
}

void Tetromino::drawNextTetromino(TerminalManager &terminal, int row, int col,
                                  int width, int height) const {
  for (int r = 0; r < height; ++r) {
    for (int c = 0; c < width; ++c) {
      terminal.drawPixel(row + r, col + c, 0);
    }
  }

  terminal.drawString(row, col, 3, "Next:");
  const std::vector<std::vector<int>> &shape = this->getShape();
  for (int r = 0; r < shape.size(); ++r) {
    for (int c = 0; c < shape[r].size(); ++c) {
      if (shape[r][c]) {
        terminal.drawPixel(row + r + 1, col + c, this->getColor());
      }
    }
  }
}

int calculateTickRate(int level) {
  if (level >= 29) {
    return LEVEL_SPEEDS[29];
  }
  return LEVEL_SPEEDS[level];
}
