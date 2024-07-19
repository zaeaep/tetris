#pragma once

#include "TerminalManager.h"
#include "Tetromino.h"
#include <vector>

std::vector<std::vector<int>> initializeField(int width, int height);

void drawFieldWithFixedBorders(TerminalManager &terminal,
                               const std::vector<std::vector<int>> &field);
// void placeTetrominoInField(vector<vector<int>> &field, const Tetromino&
// tetromino);

const Color COLOR_BLACK(0.0, 0.0, 0.0);
const Color COLOR_RED(1.0, 0.0, 0.0);
const Color COLOR_GREEN(0.0, 1.0, 0.0);
const Color COLOR_BLUE(0.0, 0.0, 1.0);
const Color COLOR_YELLOW(1.0, 1.0, 0.0);
