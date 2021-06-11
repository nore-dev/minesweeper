#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <unordered_map>

const int MINE = 10;
const int EMPTY = 0;

enum CellState
{
  INVISIBLE,
  VISIBLE,
  FLAGGED
};

#ifdef __MINGW32__
std::string to_string(const int &n)
{
  std::ostringstream stm;
  stm << n;
  return stm.str();
}
#endif

class Minesweeper
{
private:
  int *_cells = nullptr;
  CellState *_cellStates = nullptr;

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

  std::unordered_map<int, sf::Color> colors;

  int _foundMines = 0;
  int _flaggedCellsCount = 0;
  int _mineCount = 0;

  bool _gameOver = false;

public:
  Minesweeper(int rows, int cols, int size, int mineCount, sf::Font &font)
  {
    _cells = new int[rows * cols]();
    _cellStates = new CellState[rows * cols]();

    _rows = rows;
    _cols = cols;
    _size = size;
    _mineCount = mineCount;

    srand(time(NULL));

    // Load Sprites
    mineTexture.loadFromFile("res/mine.png");
    mineSprite.setTexture(mineTexture);
    mineSprite.setScale(sf::Vector2f(0.5, 0.5));

    cellTexture.loadFromFile("res/cell.png");
    cellSprite.setTexture(cellTexture);
    cellSprite.setScale(sf::Vector2f(0.5, 0.5));

    flagTexture.loadFromFile("res/flag.png");
    flagSprite.setTexture(flagTexture);
    flagSprite.setScale(sf::Vector2f(0.5, 0.5));

    colors[1] = sf::Color::Blue;
    colors[2] = sf::Color::Red;
    colors[3] = sf::Color::Black;
    colors[4] = sf::Color::White;
    colors[5] = sf::Color::Green;
    colors[6] = sf::Color::Red;
    colors[7] = sf::Color::Blue;
    colors[8] = sf::Color::Yellow;

    rect.setSize(sf::Vector2f(size, size));
    rect.setOutlineThickness(1);

    text.setFont(font);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setCharacterSize(size / 2);
  }

  ~Minesweeper()
  {
    delete _cells;
    _cells = nullptr;
    delete _cellStates;
    _cellStates = nullptr;
  }

  bool inBounds(int x, int y)
  {
    return x >= 0 && x <= _cols - 1 && y >= 0 && y <= _rows - 1;
  }

  int getIndex(int x, int y)
  {
    return y * _cols + x;
  }

  void floodFill(int x, int y)
  {
    if (!inBounds(x, y))
      return;

    int index = getIndex(x, y);

    if (_cells[index] == MINE)
      return;

    if (_cellStates[index] == INVISIBLE)
    {
      _cellStates[index] = VISIBLE;

      if (_cells[index] == EMPTY)
      {
        floodFill(x - 1, y);
        floodFill(x + 1, y);
        floodFill(x, y - 1);
        floodFill(x, y + 1);
      }
    }
  }

  void addMines(int firstIndex)
  {
    int mines = _mineCount;
    while (mines > 0)
    {
      int x = rand() % (_cols - 1);
      int y = rand() % (_rows - 1);
      sf::Vector2i pos = randomPlace(x, y, firstIndex);
      int index = getIndex(pos.x, pos.y);

      _cells[index] = MINE;
      setNeighbours(pos.x, pos.y);

      mines--;
    }
  }

  sf::Vector2i randomPlace(int x, int y, int firstIndex)
  {
    if (getCell(x, y) != MINE && getIndex(x, y) != firstIndex)
      return sf::Vector2i(x, y);

    int newX = rand() % (_cols - 1);
    int newY = rand() % (_rows - 1);
    return randomPlace(newX, newY, firstIndex);
  }

  void setNeighbours(int x, int y)
  {

    for (int ny = y - 1; ny <= y + 1; ny++)
      for (int nx = x - 1; nx <= x + 1; nx++)
      {

        if (!inBounds(nx, ny))
          continue;

        if (nx == x && ny == y)
          continue;

        int index = getIndex(nx, ny);

        if (_cells[index] != MINE)
          _cells[index]++;
      }
  }

  void setCellState(int x, int y, CellState state)
  {
    _cellStates[getIndex(x, y)] = state;
  }

  CellState getCellState(int x, int y)
  {
    return _cellStates[getIndex(x, y)];
  }

  int getCell(int x, int y)
  {
    return _cells[getIndex(x, y)];
  }

  void mouseLeftPressed(int x, int y, int cell, CellState cellState)
  {
    if (cellState == INVISIBLE)
    {
      if (cell == EMPTY)
        floodFill(x, y);

      if (cell == MINE)
        _gameOver = true;

      setCellState(x, y, VISIBLE);
    }

    if (cellState == VISIBLE)
      revealNeighbours(x, y);
  }

  bool isGameOver()
  {
    bool gameOver = _foundMines == _mineCount || _gameOver;
    if (gameOver)
      revealAllCells();

    return gameOver;
  }

  void mouseRightPressed(int x, int y, int cell, CellState cellState)
  {
    if (cellState == INVISIBLE && _flaggedCellsCount <= _mineCount)
    {

      if (cell == MINE)
        _foundMines++;

      _flaggedCellsCount++;
      setCellState(x, y, FLAGGED);
    }

    if (cellState == FLAGGED)
    {
      if (cell == MINE)
        _foundMines--;
      _flaggedCellsCount--;

      setCellState(x, y, INVISIBLE);
    }
  }

  void revealAllCells()
  {
    std::fill_n(_cellStates, _rows * _cols, VISIBLE);
  }

  void revealNeighbours(int x, int y)
  {
    // :(
    int mineCount = 0;

    int cell = getCell(x, y);

    for (int ny = y - 1; ny <= y + 1; ny++)
      for (int nx = x - 1; nx <= x + 1; nx++)
        if (getCellState(nx, ny) == FLAGGED)
        {
          if (getCell(nx, ny) == MINE)
            mineCount++;
          else
            _gameOver = true;
        }

    if (mineCount == cell)
      for (int ny = y - 1; ny <= y + 1; ny++)
        for (int nx = x - 1; nx <= x + 1; nx++)
          if (getCell(nx, ny) != MINE)
            setCellState(nx, ny, VISIBLE);
  }

  void draw(sf::RenderWindow &window)
  {
    for (int y = 0; y < _rows; y++)
      for (int x = 0; x < _cols; x++)
      {
        int cell = getCell(x, y);
        int cellState = getCellState(x, y);

        // :(
        rect.setPosition(x * _size, y * _size);
        text.setPosition(x * _size + _size / 4, y * _size + _size / 4);
        mineSprite.setPosition(x * _size, y * _size);
        cellSprite.setPosition(x * _size, y * _size);
        flagSprite.setPosition(x * _size, y * _size);

        if (cellState == VISIBLE)
        {
          rect.setFillColor(sf::Color::Transparent);
          rect.setOutlineColor(sf::Color(108, 104, 110, 255));

          text.setFillColor(colors[cell]);

#ifdef __MINGW32__
          text.setString(to_string(cell));
#else
          text.setString(std::to_string(cell));
#endif

          window.draw(rect);

          if (cell == MINE)
            window.draw(mineSprite);

          else if (cell != EMPTY)
            window.draw(text);
        }

        if (cellState == INVISIBLE)
          window.draw(cellSprite);

        if (cellState == FLAGGED)
        {
          window.draw(cellSprite);
          window.draw(flagSprite);
        }
      }
  }
};

#endif