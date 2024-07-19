#include "Tetris.h"
#include "./TerminalManager.h"
#include "./Tetromino.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 20;
const int BORDER_COLOR = 1;
// const int TICK_RATE_MS = 500;
const int LEVEL_SPEEDS[] = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6,
                            5,  5,  5,  4,  4,  4,  3,  3,  3, 2,
                            2,  2,  2,  2,  2,  2,  2,  2,  2, 1};
const int FRAMES_PER_SECOND = 60;

vector<vector<int>> initializeField(int width, int height) {
  vector<vector<int>> field(height, vector<int>(width, 0));
  return field;
}

void drawFieldWithFixedBorders(TerminalManager &terminal,
                               const vector<vector<int>> &field) {
  for (int row = 0; row < FIELD_HEIGHT + 2; row++) {
    for (int col = 0; col < FIELD_WIDTH + 2; col++) {
      if (row == 0 || row == FIELD_HEIGHT + 1 || col == 0 ||
          col == FIELD_WIDTH + 1) {
        terminal.drawPixel(row, col, BORDER_COLOR);
      } else {
        int color = field[row - 1][col - 1];
        terminal.drawPixel(row, col, color);
      }
    }
  }
  terminal.refresh();
}

int main() {
  vector<vector<int>> field = initializeField(FIELD_WIDTH, FIELD_HEIGHT);

  std::vector<std::pair<Color, Color>> colors = {
      {Color(1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0)}, // Black
      {Color(1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0)}, // Red on Blue
      {Color(1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0)}, // Green on Yellow
      {Color(1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0)}, // Blue on Red
      {Color(1.0, 1.0, 0.0), Color(0.0, 1.0, 0.0)}, // Yellow on Green
      {Color(1.0, 0.0, 1.0), Color(0.0, 1.0, 1.0)}, // Magenta on Cyan
      {Color(0.0, 1.0, 1.0), Color(1.0, 0.0, 1.0)}, // Cyan on Magenta
      {Color(1.0, 1.0, 1.0), Color(0.0, 0.0, 0.0)}, // White on Black
      {Color(0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0)}, // Black on White
  };

  TerminalManager terminal(colors);

  int totalLinesCleared = 0;
  int linesClearedAtOnce = 0;
  int level = 0;

  Tetrominos tetrominos;
  Tetromino currentTetromino = tetrominos.getRandomTetromino();
  Tetromino nextTetromino = tetrominos.getRandomTetromino();
  currentTetromino.setPosition(
      0, FIELD_WIDTH / 2 - currentTetromino.getShape()[0].size() / 2);

  bool running = false;
  auto lastTick = std::chrono::steady_clock::now();
  int TICK_RATE_MS = calculateTickRate(level);
  // Zeichne das nächste Tetromino einmal außerhalb der Schleife
  nextTetromino.drawNextTetromino(terminal, 2, FIELD_WIDTH + 5, 4, 4);

  while (!running) {
    terminal.drawString(0, FIELD_WIDTH + 5, 0,
                        ("Level: " + std::to_string(level)).c_str());
    updateTetromino(currentTetromino, tetrominos, field, lastTick, running,
                    FIELD_WIDTH, TICK_RATE_MS, totalLinesCleared,
                    linesClearedAtOnce, nextTetromino, terminal, level);

    vector<vector<int>> tempField = field;
    placeTetrominoInField(tempField, currentTetromino);
    drawFieldWithFixedBorders(terminal, tempField);

    UserInput userinput = terminal.getUserInput();
    int dx = 0;
    int dy = 0;
    if (userinput.isEscape()) {
      running = true;
    } else if (userinput.isKeyLeft()) {
      dx = -1;
    } else if (userinput.isKeyRight()) {
      dx = 1;
    } else if (userinput.isKeyDown()) {
      dy = 1;
    } else if (userinput.pressS()) {
      currentTetromino.rotateClockwise(field);
    } else if (userinput.pressA()) {
      currentTetromino.rotateCounterClockwise(field);
    }

    if (!currentTetromino.move(dx, dy, field)) {
      if (dy > 0) {
        placeTetrominoInField(field, currentTetromino);
        checkAndRemoveFullLines(field, totalLinesCleared, linesClearedAtOnce);
        currentTetromino = nextTetromino; // Verwende das nächste Tetromino
        currentTetromino.setPosition(
            0, FIELD_WIDTH / 2 - currentTetromino.getShape()[0].size() / 2);
        nextTetromino =
            tetrominos
                .getRandomTetromino(); // Generiere ein neues nächstes Tetromino
        if (!currentTetromino.move(0, 0, field)) {
          running = true;
        }
        // Zeichne das nächste Tetromino nach dem Setzen des aktuellen
        // Tetrominos
        nextTetromino.drawNextTetromino(terminal, 2, FIELD_WIDTH + 5, 4, 4);
      }
    }
  }

  return 0;
}
