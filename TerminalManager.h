// Copyright 2024, University of Freiburg,
// Chair of Algorithms and Data Structures
// Authors: Hannah Bast <bast@cs.uni-freiburg.de>
//          Johannes Kalmbach <kalmbach@cs.uni-freiburg.de>

#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

// Class to represent an RGB color.
class Color {
private:
  float red_;
  float green_;
  float blue_;

public:
  // Constructor. Each of the values `red`, `green`, `blue` must be >= 0 and
  // <= 1.
  Color(float red, float green, float blue)
      : red_{red}, green_{green}, blue_{blue} {
    auto isValid = [](float color) { return color >= 0.0 && color <= 1.0; };
    if (!isValid(red_) || !isValid(green_) || !isValid(blue_)) {
      throw std::runtime_error(
          "Invalid value for color component. Must be between 0 and 1");
    }
  }
  // Get the value of the red/green/blue component.
  float red() const { return red_; }
  float green() const { return green_; }
  float blue() const { return blue_; }
};

// Class to represent user input (key or mouse events).
class UserInput {
public:
  // Functions that check for particular keys.
  bool isEscape() const;
  bool isKeyLeft() const;
  bool isKeyRight() const;
  bool isKeyUp() const;
  bool isKeyDown() const;
  bool isMouseclick() const;
  bool pressA() const;
  bool pressS() const;
  // The code of the key that was pressed.
  int keycode_;
  int mouseRow_ = -1;
  int mouseCol_ = -1;
};

// A class to draw pixels on or read input from the terminal, using ncurses.
class TerminalManager {
public:
  // Constructor: Set up the terminal for use with ncurses commands.
  // The argument specifies the colors that we want to use with this terminal
  // manager: Each pair consists of [foreground color, background color]. The
  // `i-th` color pair in the vector can then later be chosen if `i` is
  // specified as the color argument to `drawPixel` or `drawString`.
  TerminalManager(const std::vector<std::pair<Color, Color>> &colors);

  // Destructor: Clean up the terminal after use.
  ~TerminalManager();

  // Draw a pixel at the given logical position in the given color.
  // Note: the pixel is drawn with the foreground color of the
  // color pair with the given index that was specified in the constructor.
  void drawPixel(int row, int col, int color);

  // Draw a string at the given logical position and color.
  void drawString(int row, int col, int color, const char *str);

  // Show the contents of the screen.
  void refresh();

  // Return the logical dimensions of the screen.
  int numRows() { return numRows_; }
  int numCols() { return numCols_; }

  // Get user input.
  UserInput getUserInput();

private:
  // The logical dimensions of the screen.
  int numRows_;
  int numCols_;
  int numColors_;
};
