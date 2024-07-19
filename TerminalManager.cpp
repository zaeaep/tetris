// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures
// Author: Hannah Bast <bast@cs.uni-freiburg.de>

#include "./TerminalManager.h"
#include <ncurses.h>

static constexpr size_t systemColors = 16;

// NOTE: We need `ncurses` stuff only in the implementation of
// `TerminalManager`, nowhere else (not even in `TerminalManager.h`, let alone
// anywhere in the files implementing the game logic).

// ____________________________________________________________________________
bool UserInput::isEscape() const { return keycode_ == 27; }
bool UserInput::isKeyLeft() const { return keycode_ == KEY_LEFT; }
bool UserInput::isKeyRight() const { return keycode_ == KEY_RIGHT; }
bool UserInput::isKeyUp() const { return keycode_ == KEY_UP; }
bool UserInput::isKeyDown() const { return keycode_ == KEY_DOWN; }
bool UserInput::isMouseclick() const { return mouseRow_ != -1; }
bool UserInput::pressA() const { return keycode_ == 'a'; }
bool UserInput::pressS() const { return keycode_ == 's'; }

// ____________________________________________________________________________
TerminalManager::TerminalManager(
    const std::vector<std::pair<Color, Color>> &colors)
    : numColors_(colors.size()) {
  // Initialize ncurses and some settings suitable for gaming.
  initscr();
  cbreak();
  noecho();
  curs_set(false);
  nodelay(stdscr, true);
  keypad(stdscr, true);
  // Catch mouse events
  mousemask(ALL_MOUSE_EVENTS, NULL);
  mouseinterval(0);

  // Initialize some colors.
  start_color();
  if (COLORS - systemColors < colors.size() * 2) {
    endwin();
    throw std::runtime_error{
        "The TerminalManager requires a terminal with"
        " at least 200 colors. Consider setting `TERM=xterm-256color` before"
        " starting the application"};
  }
  // Define as many shades of a certain color (red in this case) as there are
  // colors.
  auto initColor = [](int i, const Color &color) {
    int r = 1000 * color.red();
    int g = 1000 * color.green();
    int b = 1000 * color.blue();
    init_color(i, r, g, b);
  };
  for (size_t i = 0; i < numColors_; ++i) {
    const auto &[fgColor, bgColor] = colors.at(i);
    initColor(2 * (i + systemColors), fgColor);
    initColor(2 * (systemColors + i) + 1, bgColor);
    init_pair(i + systemColors, 2 * (systemColors + i),
              2 * (systemColors + i) + 1);
  }
  // Set the logical dimensions of the screen.
  numRows_ = LINES;
  numCols_ = COLS / 2;
}

// ____________________________________________________________________________
TerminalManager::~TerminalManager() { endwin(); }

// ____________________________________________________________________________
void TerminalManager::refresh() { ::refresh(); }

// ____________________________________________________________________________
void TerminalManager::drawPixel(int row, int col, int color) {
  if (color >= numColors_) {
    throw std::runtime_error("Invalid color given to drawPixel");
  }
  attron(COLOR_PAIR(color + systemColors));
  attron(A_REVERSE);
  mvprintw(row, 2 * col, "  ");
  attroff(A_REVERSE);
}

// ____________________________________________________________________________
UserInput TerminalManager::getUserInput() {
  UserInput userInput;
  userInput.keycode_ = getch();
  MEVENT event;
  if ((userInput.keycode_ == KEY_MOUSE) && (getmouse(&event) == OK)) {
    if (event.bstate & BUTTON1_PRESSED) {
      userInput.mouseRow_ = event.y;
      userInput.mouseCol_ = event.x / 2;
    }
  }
  return userInput;
}

// ____________________________________________________________________________
void TerminalManager::drawString(int row, int col, int color, const char *str) {
  if (color >= numColors_) {
    throw std::runtime_error("Invalid color given to drawString");
  }
  attron(COLOR_PAIR(color + systemColors));
  mvprintw(row, 2 * col, "%s", str);
}
