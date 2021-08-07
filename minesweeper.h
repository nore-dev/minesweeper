#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <unordered_map>

const int MINE = 9;
const int EMPTY = 0;

enum CellState
{
  INVISIBLE,
  VISIBLE,
  FLAGGED
};

class Minesweeper
{
private:
  int *_cells = nullptr;

  int _rows = 0;
  int _cols = 0;
  int _size = 0;

  sf::RectangleShape rect;
  sf::Text text;

  sf::Texture mineTexture;
  sf::Texture flagTexture;
  sf::Texture cellTexture;

  sf::Sprite mineSprite;
  sf::Sprite flagSprite;
  sf::Sprite cellSprite;

  sf::Color colors[8] = {
      sf::Color::Blue, sf::Color::Red, sf::Color::Black, sf::Color::White, // 1 2 3 4
      sf::Color::Green, sf::Color::Red, sf::Color::Blue, sf::Color::Yellow // 5 6 7 8
  };

  int _foundMines = 0;
  int _flaggedCellsCount = 0;
  int _mineCount = 0;

  bool _gameOver = false;

public:
  Minesweeper(int rows, int cols, int size, int mineCount, sf::Font &font);
  ~Minesweeper();

  bool inBounds(int x, int y);
  int getIndex(int x, int y);
  void floodFill(int x, int y);
  void addMines(int firstIndex);
  sf::Vector2i randomPlace(int x, int y, int firstIndex);
  void setNeighbours(int x, int y);
  void setCellState(int x, int y, CellState state);
  CellState getCellState(int x, int y);
  int getCell(int x, int y);
  void mouseLeftPressed(int x, int y);
  bool isGameOver();
  void mouseRightPressed(int x, int y);
  void revealAllCells();
  void revealNeighbours(int x, int y);
  void draw(sf::RenderWindow &window);
  void reset();
};

#endif