#include "minesweeper.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <unordered_map>

Minesweeper::Minesweeper(int rows, int cols, int size, int mineCount, sf::Font &font)
{
  _cells = new int[rows * cols]();

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

  rect.setSize(sf::Vector2f(size, size));
  rect.setOutlineThickness(1);

  text.setFont(font);
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width / 2.0f,
                 textRect.top + textRect.height / 2.0f);
  text.setCharacterSize(size / 2);
}

Minesweeper::~Minesweeper()
{
  delete _cells;
  _cells = nullptr;
}

void Minesweeper::draw(sf::RenderWindow &window)
{
  for (int y = 0; y < _rows; y++)
    for (int x = 0; x < _cols; x++)
    {
      int cell = getCell(x, y);
      CellState cellState = getCellState(x, y);

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

        text.setFillColor(colors[cell - 1]);
        text.setString(std::to_string(cell));

        window.draw(rect);

        if (cell == MINE)
          window.draw(mineSprite);

        if (cell != EMPTY)
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

void Minesweeper::revealNeighbours(int x, int y)
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
      {
        if (getCell(nx, ny) == EMPTY)
          floodFill(nx, ny);

        if (getCell(nx, ny) != MINE)
          setCellState(nx, ny, VISIBLE);
      }
}

void Minesweeper::revealAllCells()
{
  for (int i = 0; i < _rows; i++)
    for (int j = 0; j < _cols; j++)
    {
      setCellState(j, i, VISIBLE);
    }
}

void Minesweeper::mouseRightPressed(int x, int y)
{
  CellState cellState = getCellState(x, y);
  if (cellState == INVISIBLE && _flaggedCellsCount <= _mineCount)
  {

    if (getCell(x, y) == MINE)
      _foundMines++;

    _flaggedCellsCount++;
    setCellState(x, y, FLAGGED);
  }

  if (cellState == FLAGGED)
  {
    if (getCell(x, y) == MINE)
      _foundMines--;
    _flaggedCellsCount--;

    setCellState(x, y, INVISIBLE);
  }
}

bool Minesweeper::isGameOver()
{
  bool gameOver = _foundMines == _mineCount || _gameOver;
  if (gameOver)
    revealAllCells();

  return gameOver;
}

void Minesweeper::mouseLeftPressed(int x, int y)
{
  if (getCellState(x, y) == INVISIBLE)
  {
    if (getCell(x, y) == EMPTY)
      floodFill(x, y);

    if (getCell(x, y) == MINE)
      _gameOver = true;

    setCellState(x, y, VISIBLE);
  }

  if (getCellState(x, y) == VISIBLE && getCell(x, y) != EMPTY)
    revealNeighbours(x, y);
}

sf::Vector2i Minesweeper::randomPlace(int x, int y, int firstIndex)
{
  if (getCell(x, y) != MINE && getIndex(x, y) != firstIndex)
    return sf::Vector2i(x, y);

  int newX = rand() % (_cols - 1);
  int newY = rand() % (_rows - 1);
  return randomPlace(newX, newY, firstIndex);
}

void Minesweeper::setNeighbours(int x, int y)
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

void Minesweeper::setCellState(int x, int y, CellState state)
{
  _cells[getIndex(x, y)] = getCell(x, y) + state * 10;
}

CellState Minesweeper::getCellState(int x, int y)
{
  return (CellState)(_cells[getIndex(x, y)] / 10);
}

int Minesweeper::getCell(int x, int y)
{
  return _cells[getIndex(x, y)] % 10;
}

bool Minesweeper::inBounds(int x, int y)
{
  return x >= 0 && x <= _cols - 1 && y >= 0 && y <= _rows - 1;
}

int Minesweeper::getIndex(int x, int y)
{
  return y * _cols + x;
}

void Minesweeper::floodFill(int x, int y)
{
  if (!inBounds(x, y))
    return;

  if (getCell(x, y) == MINE)
    return;

  if (getCellState(x, y) == INVISIBLE)
  {
    setCellState(x, y, VISIBLE);

    if (getCell(x, y) == EMPTY)
    {
      floodFill(x - 1, y);
      floodFill(x + 1, y);
      floodFill(x, y - 1);
      floodFill(x, y + 1);
    }
  }
}

void Minesweeper::addMines(int firstIndex)
{
  int mines = _mineCount;
  while (mines > 0)
  {
    int x = rand() % (_cols - 1);
    int y = rand() % (_rows - 1);
    sf::Vector2i pos = randomPlace(x, y, firstIndex);

    _cells[getIndex(pos.x, pos.y)] = MINE;
    setNeighbours(pos.x, pos.y);

    mines--;
  }
}

void Minesweeper::reset()
{
  _flaggedCellsCount = 0;
  _gameOver = false;
  _foundMines = 0;
  _cells = new int[_rows * _cols]();
}